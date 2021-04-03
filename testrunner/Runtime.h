#pragma once
#include <chaiscript/chaiscript.hpp>
#include <functional>
#include <modernOpc/client/Client.h>
#include <spdlog/spdlog.h>

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
   std::function<modernopc::Variant(const modernopc::UnresolvedNodeId&)> m_read;
   std::function<void(const modernopc::UnresolvedNodeId& id, const modernopc::Variant& var)> m_write;
};
} // namespace opctest::testrunner