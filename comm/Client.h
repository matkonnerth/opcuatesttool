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
#include <optional>
#include <variant>

struct UA_Client;
namespace tt {

using Variant = std::variant<bool, int32_t, uint32_t, float, double, std::string>;

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

   std::optional<Variant> read(const NodeId& id);

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