#pragma once
#include <chaiscript/chaiscript.hpp>

namespace tt
{
class Runtime
{
public:
   Runtime(const std::string& uri, const std::string& script)
   : m_uri{ uri }, m_script{ script }
   {}
   bool load();
   void eval();
private:
   std::string m_uri;
   std::string m_script;
   chaiscript::ChaiScript chai;
};
}