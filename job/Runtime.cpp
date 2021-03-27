#include "Runtime.h"
#include "../comm/Client.h"
#include "../comm/Comm.h"
#include <chrono>
#include <thread>


using tt::Runtime;

void wait(int delay)
{
   std::this_thread::sleep_for(std::chrono::milliseconds(delay));
}

bool Runtime::load()
{
   tt::Comm comm;
   auto client = comm.createTestClient(m_uri);
   if(!client->connect())
   {
       return false;
   }
   chai.add(chaiscript::fun(&wait), "wait");
   return true;
}

void Runtime::eval()
{
   chai.eval(m_script);
}