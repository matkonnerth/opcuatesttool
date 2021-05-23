#include "Runtime.h"
#include <chrono>
#include <iostream>
#include <memory>
#include <modernOpc/types/NodeId.h>
#include <modernOpc/Variant.h>
#include <modernOpc/BrowseResult.h>
#include <string>
#include <thread>

using modernopc::BrowseResult;
using modernopc::NodeId;
using modernopc::Variant;
using opctest::testrunner::Runtime;
using modernopc::Client;

void wait(int delay)
{
   std::this_thread::sleep_for(std::chrono::milliseconds(delay));
}

std::vector<Variant> VariantVector()
{
   return {};
}

void Runtime::load()
{
   client = std::make_unique<modernopc::Client>(m_uri);
   client->connect();

   lua.open_libraries(sol::lib::base, sol::lib::string);
   lua.do_file(m_scriptDir + "/libOpc.lua");

   lua.set_function("connect", &Client::connect, client.get());
   lua.set_function("disconnect", &Client::disconnect, client.get());
   lua.set_function("read", &Client::read, client.get());
   lua.set_function("write", &Client::write, client.get());


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

   lua.set_function("resolveUri", [&](const std::string& uri) { return client->resolveNamespaceUri(uri);});

   lua.set_function("call", &Client::call, client.get());
   lua.set_function("browse", &Client::browse, client.get());

   lua.set_function("VariantVector", &VariantVector);
}

void Runtime::eval()
{
   try
   {
      lua.script_file(m_scriptDir + "/" + m_script);
   }
   catch (const std::exception& e)
   {
      logger->error(e.what());
      throw;
   }
}