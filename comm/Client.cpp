#include "Client.h"
#include <open62541/client.h>
#include <open62541/client_config_default.h>
#include <open62541/client_highlevel.h>
#include <open62541/client_highlevel_async.h>
#include <open62541/client_subscriptions.h>
#include <open62541/plugin/log_stdout.h>


namespace tt
{
Client::Client(const std::string& endpointUri)
{
   uri = endpointUri;
   client = UA_Client_new();
   UA_ClientConfig *cc = UA_Client_getConfig(client);
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
    printf("in Client::connect\n\n");
     UA_StatusCode retval = UA_Client_connect(client, uri.c_str());
    if(retval != UA_STATUSCODE_GOOD) {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                    "Could not connect client %s with %s", uri.c_str(),
                    UA_StatusCode_name(retval));
        return false;
    }

    UA_NodeId namespaceArrayId = UA_NODEID_NUMERIC(0, UA_NS0ID_SERVER_NAMESPACEARRAY);
    UA_Variant var;
    UA_Variant_init(&var);
    auto status = UA_Client_readValueAttribute(client, namespaceArrayId, &var);
    if(status!=UA_STATUSCODE_GOOD)
    {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                    "could not read namespaceArray with %s",
                    UA_StatusCode_name(retval));
        return false;
    }
    for(auto i=0u; i<var.arrayLength; i++)
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
    if(!status==UA_STATUSCODE_GOOD)
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
    for(auto& cb : connectionStateCallbacks)
    {
        cb(this, state);
    }
}

bool Client::cacheNodeId(const NodeId& id)
{
    bool found = false;
    size_t idx = 0;
    for(const auto& nsUri:namespaces)
    {
        if(!id.namespaceUri.compare(nsUri))
        {
            found = true;
            break;
        }
        idx++;
    }
    if(!found)
    {
        return false;
    }
    UA_NodeId newId;
    UA_NodeId_init(&newId);
    auto status = UA_NodeId_parse(&newId, UA_STRING_ALLOC(id.identifier.data()));
    if(status!=UA_STATUSCODE_GOOD)
    {
        return false;
    }
    nodeIdCache.emplace(id, newId);
    return true;
}

bool Client::read(const NodeId& id)
{
    auto uaId = nodeIdCache.find(id);
    if(uaId==nodeIdCache.end())
    {
        return false;
    }

    UA_Variant var;
    UA_Variant_init(&var);
    auto status = UA_Client_readValueAttribute(client, uaId->second, &var);
    if(status!=UA_STATUSCODE_GOOD)
    {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND,
                    "could not read with NodeId %s",
                    UA_StatusCode_name(status));
        return false;
    }
    UA_Variant_clear(&var);
    return true;
}

Client::ConnectionState Client::getConnectionState()
{
    return connState;
}
}