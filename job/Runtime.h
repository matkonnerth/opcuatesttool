#pragma once
#include "../comm/Client.h"
#include "../comm/Comm.h"
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
   std::shared_ptr<Client> client{};
   tt::Comm comm{};
};
}