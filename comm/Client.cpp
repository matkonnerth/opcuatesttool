#include "Client.h"
#include <open62541/client.h>
#include <open62541/client_config_default.h>
#include <open62541/client_highlevel.h>
#include <open62541/client_highlevel_async.h>
#include <open62541/client_subscriptions.h>
#include <open62541/plugin/log_stdout.h>



namespace tt {
Client::Client(const std::string& endpointUri)
{
   uri = endpointUri;
   client = UA_Client_new();
   UA_ClientConfig* cc = UA_Client_getConfig(client);
   UA_ClientConfig_setDefault(cc);
   cc->clientContext = this;
}

Client::~Client()
{
   disconnect();
   UA_Client_delete(client);
}

bool Client::connect()
{
   UA_StatusCode retval = UA_Client_connect(client, uri.c_str());
   if (retval != UA_STATUSCODE_GOOD)
   {
      UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Could not connect client %s with %s", uri.c_str(), UA_StatusCode_name(retval));
      return false;
   }

   UA_NodeId namespaceArrayId = UA_NODEID_NUMERIC(0, UA_NS0ID_SERVER_NAMESPACEARRAY);
   UA_Variant var;
   UA_Variant_init(&var);
   auto status = UA_Client_readValueAttribute(client, namespaceArrayId, &var);
   if (status != UA_STATUSCODE_GOOD)
   {
      UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "could not read namespaceArray with %s", UA_StatusCode_name(retval));
      return false;
   }
   for (auto i = 0u; i < var.arrayLength; i++)
   {
      auto s = static_cast<UA_String*>(var.data)[i];
      std::string str;
      namespaces.push_back(str.assign(reinterpret_cast<char*>(s.data), s.length));
   }
   UA_Variant_clear(&var);
   connState = ConnectionState::CONNECTED;
   return true;
}

void Client::disconnect()
{
   UA_Client_disconnect(client);
}

void Client::doComm()
{
   auto status = UA_Client_connect(client, uri.c_str());
   if (!status == UA_STATUSCODE_GOOD)
   {
      UA_sleep_ms(100);
      return;
   }
   UA_Client_run_iterate(client, 0);
}


void Client::registerConnectionCallback(ConnectionStateCallback fn)
{
   connectionStateCallbacks.emplace_back(fn);
}

void Client::notifyConnectionState(ConnectionState state)
{
   for (auto& cb : connectionStateCallbacks)
   {
      cb(this, state);
   }
}

bool Client::cacheNodeId(const NodeId& id)
{
   bool found = false;
   size_t idx = 0;
   for (const auto& nsUri : namespaces)
   {
      if (!id.namespaceUri.compare(nsUri))
      {
         found = true;
         break;
      }
      idx++;
   }
   if (!found)
   {
      return false;
   }
   UA_NodeId newId;
   UA_NodeId_init(&newId);
   
   auto status = UA_NodeId_parse(&newId, UA_STRING_ALLOC(id.identifier.data()));
   newId.namespaceIndex = static_cast<UA_UInt16>(idx);
   if (status != UA_STATUSCODE_GOOD)
   {
      return false;
   }
   nodeIdCache.emplace(id, newId);
   return true;
}

Client::ConnectionState Client::getConnectionState()
{
   return connState;
}

ReadValueResult TestClient::read(const NodeId& id)
{
   ReadValueResult result{};
   result.ok=false;
   auto uaId = nodeIdCache.find(id);
   if (uaId == nodeIdCache.end())
   {
      return result;
   }
   auto status = UA_Client_readValueAttribute(client, uaId->second, result.var->getVariant());
   if (status != UA_STATUSCODE_GOOD)
   {
      UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "could not read with NodeId %s", UA_StatusCode_name(status));
      return result;
   }
   result.ok=true;
   return result;
}

bool TestClient::browse(const NodeId& id)
{
   auto uaId = nodeIdCache.find(id);
   if (uaId == nodeIdCache.end())
   {
      return false;
   }

   UA_BrowseRequest bReq;
   UA_BrowseRequest_init(&bReq);
   bReq.requestedMaxReferencesPerNode = 0;
   bReq.nodesToBrowse = UA_BrowseDescription_new();
   bReq.nodesToBrowseSize = 1;
   bReq.nodesToBrowse[0].nodeId = uaId->second;
   bReq.nodesToBrowse[0].resultMask = UA_BROWSERESULTMASK_ALL; /* return everything */
   UA_BrowseResponse bResp = UA_Client_Service_browse(client, bReq);
   auto status = true;
   if (bResp.results->statusCode != UA_STATUSCODE_GOOD)
   {
      status = false;
   }

   UA_BrowseRequest_clear(&bReq);
   UA_BrowseResponse_clear(&bResp);
   return status;
} // namespace tt

static bool readRequest(UA_Client* client, const UA_ExtensionObject& serviceObj)
{
   UA_ReadResponse resp = UA_Client_Service_read(client, *static_cast<UA_ReadRequest*>(serviceObj.content.decoded.data));
   if (resp.results->status != UA_STATUSCODE_GOOD)
   {
      return false;
   }
   return true;
}

static bool callRequest(UA_Client* client, const UA_ExtensionObject& serviceObj)
{
   UA_CallResponse resp = UA_Client_Service_call(client, *static_cast<UA_CallRequest*>(serviceObj.content.decoded.data));
   if (resp.results->statusCode != UA_STATUSCODE_GOOD)
   {
      return false;
   }
   return true;
}

static bool writeRequest(UA_Client* client, const UA_ExtensionObject& serviceObj)
{
   UA_WriteResponse resp = UA_Client_Service_write(client, *static_cast<UA_WriteRequest*>(serviceObj.content.decoded.data));
   if (resp.results != UA_STATUSCODE_GOOD)
   {
      return false;
   }
   return true;
}

static bool browseRequest(UA_Client* client, const UA_ExtensionObject& serviceObj)
{
   UA_BrowseResponse resp = UA_Client_Service_browse(client, *static_cast<UA_BrowseRequest*>(serviceObj.content.decoded.data));
   if (resp.results != UA_STATUSCODE_GOOD)
   {
      return false;
   }
   return true;
}

const std::unordered_map<int, TestClient::ServiceRequestFnc> TestClient::requests = { { UA_TYPES_BROWSEREQUEST, browseRequest }, { UA_TYPES_WRITEREQUEST, writeRequest }, { UA_TYPES_READREQUEST, readRequest }, { UA_TYPES_CALLREQUEST, callRequest } };

bool TestClient::invokeGenericService(const std::string& jsonRequest)
{
   UA_ExtensionObject out;
   UA_ExtensionObject_init(&out);
   UA_ByteString buf;
   buf.data = reinterpret_cast<UA_Byte*>(const_cast<char*>(jsonRequest.c_str()));
   buf.length = jsonRequest.length();

   UA_StatusCode retval = UA_decodeJson(&buf, &out, &UA_TYPES[UA_TYPES_EXTENSIONOBJECT]);
   if (retval != UA_STATUSCODE_GOOD)
   {
      return false;
   }
   return requests.at(out.content.decoded.type->typeIndex)(client, out);
}


} // namespace tt