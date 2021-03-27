
#include <chaiscript/chaiscript.hpp>
#include "../comm/Comm.h"
#include "../comm/Client.h"

std::string helloWorld(const std::string& t_name)
{
   return "Hello " + t_name + "!";
}

void wait(int delay)
{
   std::this_thread::sleep_for(std::chrono::milliseconds(delay));
}


int main()
{
   chaiscript::ChaiScript chai;
   chai.add(chaiscript::fun(&helloWorld), "helloWorld");

   tt::Comm comm;
   auto client = comm.createTestClient("opc.tcp://localhost:4840");

   chai.add(chaiscript::fun(&tt::Client::connect, client.get()), "connect");
   chai.add(chaiscript::fun(&wait), "wait");

   chai.eval(R"(
    puts(helloWorld("eval chai"));
    for (var i = 0; i < 100; ++i) { 
        connect();
        wait(500);
    }
  )");
}