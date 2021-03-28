#include "Client.h"
#include <open62541/client.h>
#include <open62541/client_config_default.h>
#include <open62541/client_highlevel.h>
#include <open62541/client_highlevel_async.h>
#include <open62541/client_subscriptions.h>
#include <open62541/plugin/log_stdout.h>
#include "Exception.h"


namespace opctest::client {
Client::Client(const std::string& endpointUri)
{
   uri = endpointUri;
   client = UA_Client_new();
   UA_ClientConfig* cc = UA_Client_getConfig(client);
   UA_ClientConfig_setDefault(cc);
   cc->clientContext = this;
   cc->logger = logger.getUALogger();
}

Client::~Client()
{
   disconnect();
   UA_Client_delete(client);
}

void Client::connect()
{
   UA_StatusCode retval = UA_Client_connect(client, uri.c_str());
   if (retval != UA_STATUSCODE_GOOD)
   {
      UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "Could not connect client %s with %s", uri.c_str(), UA_StatusCode_name(retval));
      throw OpcException("could not connect");
   }

   UA_NodeId namespaceArrayId = UA_NODEID_NUMERIC(0, UA_NS0ID_SERVER_NAMESPACEARRAY);
   UA_Variant var;
   UA_Variant_init(&var);
   auto status = UA_Client_readValueAttribute(client, namespaceArrayId, &var);
   if (status != UA_STATUSCODE_GOOD)
   {
      UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "could not read namespaceArray with %s", UA_StatusCode_name(retval));
      throw OpcException("could not read namespace array");
   }
   for (auto i = 0u; i < var.arrayLength; i++)
   {
      auto s = static_cast<UA_String*>(var.data)[i];
      std::string str;
      namespaces.push_back(str.assign(reinterpret_cast<char*>(s.data), s.length));
   }
   UA_Variant_clear(&var);
   connState = ConnectionState::CONNECTED;
}

void Client::disconnect()
{
   UA_Client_disconnect(client);
}

void Client::doComm()
{
   auto status = UA_Client_connect(client, uri.c_str());
   if (!(status == UA_STATUSCODE_GOOD))
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

void Client::cacheNodeId(const NodeId& id)
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
      throw OpcException("namespaceUri of nodeId not found");
   }
   UA_NodeId newId;
   UA_NodeId_init(&newId);

   auto status = UA_NodeId_parse(&newId, UA_STRING_ALLOC(id.identifier.data()));
   newId.namespaceIndex = static_cast<UA_UInt16>(idx);
   if (status != UA_STATUSCODE_GOOD)
   {
      throw OpcException("could not parse nodeId");
   }
   nodeIdCache.emplace(id, newId);
}

Client::ConnectionState Client::getConnectionState()
{
   return connState;
}

Variant Client::read(const NodeId& id)
{
   cacheNodeId(id);
   auto uaId = nodeIdCache.find(id);
   if (uaId == nodeIdCache.end())
   {
      throw OpcException("NodeId not found in cache");
   }
   UA_Variant var;
   UA_Variant_init(&var);
   auto status = UA_Client_readValueAttribute(client, uaId->second, &var);
   if (status != UA_STATUSCODE_GOOD)
   {
      UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "could not read with NodeId %s", UA_StatusCode_name(status));
      throw OpcException("Error on readValueAttribute");
   }

   Variant v;
   if(UA_NodeId_equal(&var.type->typeId, &UA_TYPES[UA_TYPES_INT32].typeId))
   {
      v = *static_cast<int32_t*>(var.data);
   }
   else if (UA_NodeId_equal(&var.type->typeId, &UA_TYPES[UA_TYPES_UINT32].typeId))
   {
      v = *static_cast<uint32_t*>(var.data);
   }

   UA_Variant_clear(&var);
   return v;
}

} // namespace opctest::client
