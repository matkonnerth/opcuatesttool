#include "Runtime.h"
#include <chrono>
#include <iostream>
#include <memory>
#include <modernOpc/UnresolvedNodeId.h>
#include <modernOpc/Variant.h>
#include <modernOpc/types/NodeId.h>
#include <thread>

using opctest::testrunner::Runtime;

void wait(int delay)
{
   std::this_thread::sleep_for(std::chrono::milliseconds(delay));
}

void Runtime::load()
{

   client = std::make_unique<modernopc::Client>(m_uri);
   client->connect();

   m_read = [&](const modernopc::UnresolvedNodeId& id) {
      auto resolvedId = client->resolve(id);
      return client->read(resolvedId);
   };

   m_write =
   [&](const modernopc::UnresolvedNodeId& id, const modernopc::Variant& var) {
      auto resolvedId = client->resolve(id);
      client->write(resolvedId, var);
   };


   chai.add(chaiscript::fun(&wait), "wait");
   chai.add(chaiscript::user_type<modernopc::UnresolvedNodeId>(), "NodeId");
   chai.add(chaiscript::constructor<modernopc::UnresolvedNodeId(const std::string& ns, const std::string identifier)>(), "NodeId");
   chai.add(chaiscript::constructor<modernopc::UnresolvedNodeId(const modernopc::UnresolvedNodeId& other)>(), "NodeId");

   chai.add(chaiscript::user_type<modernopc::Variant>(), "Variant");
   chai.add(chaiscript::constructor<modernopc::Variant(int32_t val)>(), "Int32");
   chai.add(chaiscript::constructor<modernopc::Variant(int64_t val)>(), "Int64");
   chai.add(chaiscript::fun(m_read), "read");
   chai.add(chaiscript::fun(m_write), "write");
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