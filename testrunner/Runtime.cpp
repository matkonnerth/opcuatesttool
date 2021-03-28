#include "Runtime.h"
#include <chrono>
#include <thread>
#include <iostream>

using opctest::testrunner::Runtime;

void wait(int delay)
{
   std::this_thread::sleep_for(std::chrono::milliseconds(delay));
}

/*
template <class>
inline constexpr bool always_false_v = false;

std::string toString(const opctest::client::Variant& var)
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

*/



void Runtime::load()
{
   
   client = comm.createClient(m_uri);
   client->connect();
   chai.add(chaiscript::fun(&wait), "wait");
   chai.add(chaiscript::user_type<opctest::client::NodeId>(), "NodeId");
   chai.add(chaiscript::constructor<opctest::client::NodeId(const std::string& uri, const std::string& id)>(), "NodeId");
   chai.add(chaiscript::constructor<opctest::client::NodeId(const opctest::client::NodeId& other)>(), "NodeId");
   chai.add(chaiscript::fun(&opctest::client::Client::read, client.get()), "read");
   //chai.add(chaiscript::fun(&toString), "toString");
   //chai.add(chaiscript::fun(&get), "get");
}

void Runtime::eval()
{
   try
   {
      chai.eval_file(m_script);
   }
   catch (chaiscript::exception::eval_error& e)
   {
      logger->error(e.pretty_print());
      throw;
   }
}