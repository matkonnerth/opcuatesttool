#pragma once
#include <functional>
#include <sol/sol.hpp>
#include <spdlog/spdlog.h>
#include <vector>
#include <modernOpc/client/Client.h>

namespace opctest::testrunner {
class Runtime
{
public:
   Runtime(const std::string& uri, const std::string& scriptDir, const std::string& script)
   : m_uri{ uri }
   , m_scriptDir {scriptDir}
   , m_script{ script }
   , logger{ spdlog::get("TestRunner") }
   {}
   void load();
   void eval();

private:
   std::string m_uri;
   std::string m_scriptDir;
   std::string m_script;
   std::shared_ptr<spdlog::logger> logger;
   sol::state lua;
   std::shared_ptr<modernopc::Client> client{};
};
} // namespace opctest::testrunner