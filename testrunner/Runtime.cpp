#include "Runtime.h"
#include <chrono>
#include <iostream>
#include <memory>
#include <modernOpc/UnresolvedNodeId.h>
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


   chai.add(chaiscript::fun(&wait), "wait");
   chai.add(chaiscript::user_type<modernopc::UnresolvedNodeId>(), "NodeId");
   chai.add(chaiscript::constructor<modernopc::UnresolvedNodeId(const std::string& ns, const std::string identifier)>(), "NodeId");
   chai.add(chaiscript::constructor<modernopc::UnresolvedNodeId(const modernopc::UnresolvedNodeId& other)>(), "NodeId");
   chai.add(chaiscript::fun(m_read), "read");
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