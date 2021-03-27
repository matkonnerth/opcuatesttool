#pragma once
#include "Logger.h"
#include "NodeId.h"
#include <functional>
#include <map>
#include <open62541/client_config.h>
#include <open62541/types.h>
#include <string>
#include <unordered_map>
#include <vector>

struct UA_Client;
namespace tt {

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
   Client(const Client&) = delete;
   Client& operator=(const Client&) = delete;
   bool connect();
   void disconnect();
   void doComm();
   void registerConnectionCallback(ConnectionStateCallback fn);
   void notifyConnectionState(ConnectionState state);
   bool cacheNodeId(const NodeId& id);
   ConnectionState getConnectionState();

   bool read(const NodeId& id);

private:
   UA_Client* client{ nullptr };
   std::string uri{};
   std::vector<ConnectionStateCallback> connectionStateCallbacks{};
   ConnectionState connState{ ConnectionState::DISCONNECTED };
   std::map<NodeId, UA_NodeId> nodeIdCache{};
   std::vector<std::string> namespaces{};
   Logger logger{};
};

} // namespace tt