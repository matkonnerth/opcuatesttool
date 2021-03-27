#include "Runtime.h"
#include <chrono>
#include <thread>


using tt::Runtime;

void wait(int delay)
{
   std::this_thread::sleep_for(std::chrono::milliseconds(delay));
}

template <class>
inline constexpr bool always_false_v = false;

std::string toString(const tt::Variant& var)
{
   std::visit(
   [&](auto&& arg) {
      using T = std::decay_t<decltype(arg)>;
      if constexpr (std::is_same_v<T, int>)
      {
         return std::to_string(arg);
      }
      else if constexpr (std::is_same_v<T, bool>)
      {
         return std::to_string(arg);
      }
      else if constexpr (std::is_same_v<T, uint32_t>)
      {
         return std::to_string(arg);
      }
      else if constexpr (std::is_same_v<T, std::string>)
      {
         return arg;
      }
      else if constexpr (std::is_same_v<T, float>)
      {
         return std::to_string(arg);
      }
      else if constexpr (std::is_same_v<T, double>)
      {
         return std::to_string(arg);
      }
      else
      {
         static_assert(always_false_v<T>, "non-exhaustive visitor!");
      }
   },
   var);
}

tt::Variant get(std::optional<tt::Variant> opt)
{
    return *opt;
}

bool Runtime::load()
{
   
   client = comm.createClient(m_uri);
   if(!client->connect())
   {
       return false;
   }
   chai.add(chaiscript::fun(&wait), "wait");
   chai.add(chaiscript::user_type<tt::NodeId>(), "NodeId");
   chai.add(chaiscript::constructor<tt::NodeId(const std::string& uri, const std::string& id)>(), "NodeId");
   chai.add(chaiscript::fun(&tt::Client::read, client.get()), "read");
   chai.add(chaiscript::fun(&toString), "toString");
   chai.add(chaiscript::fun(&get), "get");
   return true;
}

void Runtime::eval()
{
   chai.eval_file(m_script);
}