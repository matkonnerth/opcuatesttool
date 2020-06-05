#pragma once
#include <string>
#include <open62541/types.h>
#include <open62541/client_config.h>
#include <unordered_map>
#include <functional>
#include <vector>
#include "NodeId.h"
#include <map>

struct UA_Client;
namespace tt
{

class Client
{    
public:
    enum class ConnectionState
    {
        DISCONNECTED,
        CONNECTED
    };
    using ConnectionStateCallback = std::function<void(const Client* client, ConnectionState state)>;
    Client(const std::string& endpointUri);
    ~Client();
    Client(const Client&)=delete;
    Client& operator=(const Client&)=delete;        
    void connect();
    void disconnect();
    void doComm();
    void registerConnectionCallback(ConnectionStateCallback fn);
    void notifyConnectionState(ConnectionState state);
    bool cacheNodeId(const NodeId& id);
    bool read(const NodeId& id);

private:
    UA_Client* client{nullptr};
    std::string uri{};
    std::vector<ConnectionStateCallback> connectionStateCallbacks{};
    //static void
    //handler_events(UA_Client *client, UA_UInt32 subId, void *subContext,
    //           UA_UInt32 monId, void *monContext,
    //           size_t nEventFields, UA_Variant *eventFields);
    //static void createSubscription(UA_Client* client);
    //static void stateCallback (UA_Client *client, UA_ClientState clientState);
    std::map<NodeId, UA_NodeId> nodeIdCache {};
    std::vector<std::string> namespaces {};
};

}