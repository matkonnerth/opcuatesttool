#pragma once
#include <chaiscript/chaiscript.hpp>
#include <functional>
#include <spdlog/spdlog.h>
#include <vector>
#include <modernOpc/BrowseResult.h>
#include "Opc.h"

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
   Opc opc{};
};
} // namespace opctest::testrunner