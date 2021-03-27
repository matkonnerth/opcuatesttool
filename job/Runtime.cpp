#include "Runtime.h"
#include <chrono>
#include <thread>


using tt::Runtime;

void wait(int delay)
{
   std::this_thread::sleep_for(std::chrono::milliseconds(delay));
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
   return true;
}

void Runtime::eval()
{
   chai.eval_file(m_script);
}