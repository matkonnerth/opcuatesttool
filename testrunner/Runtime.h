#pragma once
#include <chaiscript/chaiscript.hpp>
#include <functional>
#include <modernOpc/client/Client.h>
#include <spdlog/spdlog.h>
#include <vector>
#include <modernOpc/BrowseResult.h>

namespace opctest::testrunner {
class Runtime
{
public:
   Runtime(const std::string& uri, const std::string& script)
   : m_uri{ uri }
   , m_script{ script }
   , logger{ spdlog::get("TestRunner") }
   {}
   void load();
   void eval();

private:
   std::string m_uri;
   std::string m_script;
   std::shared_ptr<spdlog::logger> logger;
   chaiscript::ChaiScript chai;
   std::shared_ptr<modernopc::Client> client{};
   std::function<modernopc::Variant(const modernopc::NodeId&)> m_read;
   std::function<void(const modernopc::NodeId& id, const modernopc::Variant& var)> m_write;
   std::function<std::vector<modernopc::BrowseResult>(const modernopc::NodeId& id)> m_browse;
   std::function<bool(const modernopc::BrowseResult& res)> m_IsVariable;
};
} // namespace opctest::testrunner