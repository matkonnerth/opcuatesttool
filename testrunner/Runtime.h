#pragma once
#include "../client/Client.h"
#include "../client/Comm.h"
#include <chaiscript/chaiscript.hpp>
#include <spdlog/spdlog.h>

namespace opctest::testrunner
{
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
   std::shared_ptr<opctest::client::Client> client{};
   opctest::client::Comm comm{};
};
}