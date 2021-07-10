#include "OpcUaExtension.h"
#include <modernopc/BrowseResult.h>
#include <modernopc/Variant.h>
#include <modernopc/types/NodeId.h>
#include <string>

using modernopc::BrowseResult;
using modernopc::Client;
using modernopc::NodeId;
using modernopc::Variant;

void OpcUaExtension::add(sol::state_view& lua)
{
   lua.set_function("createClient", [&](const std::string& uri) { client = std::make_unique<modernopc::Client>(uri); });

   lua.set_function("connect", [&](){ client->connect();});
   lua.set_function("disconnect", [&](){ client->disconnect();});
   lua.set_function("read", [&](const modernopc::NodeId& id){return client->read(id);});

   // make usertype metatable
   auto NodeId_type = lua.new_usertype<NodeId>("NodeId",
                                               // 3 constructors
                                               sol::constructors<NodeId(uint16_t nsIdx, int id), NodeId(uint16_t nsIdx, const std::string& id), NodeId(const NodeId&)>());

   auto Variant_Type = lua.new_usertype<Variant>("Variant", sol::constructors<Variant(int32_t val), Variant(int64_t val), Variant(std::string val)>());
   Variant_Type["toString"] = &Variant::toString;

   auto BrowseResult_Type = lua.new_usertype<BrowseResult>("BrowseResult", sol::constructors<BrowseResult(const BrowseResult&), BrowseResult(BrowseResult &&)>());
   BrowseResult_Type["Id"] = &BrowseResult::Id;
   BrowseResult_Type["Name"] = &BrowseResult::Name;
   lua["BrowseResult"]["IsVariable"] = [](BrowseResult& br) { return (br.Type() == modernopc::NodeType::VARIABLE); };

   lua.set_function("resolveUri", [&](const std::string& uri) { return client->resolveNamespaceUri(uri); });
}
