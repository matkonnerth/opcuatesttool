#include <pistache/description.h>
#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/router.h>
#include "JobScheduler.h"



using namespace std;
using namespace Pistache;

namespace Generic {

void handleReady(const Rest::Request&, Http::ResponseWriter response)
{
   response.send(Http::Code::Ok, "1");
}

} // namespace Generic

class TestService
{
public:
   TestService(Address addr)
   : httpEndpoint(std::make_shared<Http::Endpoint>(addr))
   {}

   void init(size_t thr = 1)
   {
      using namespace Rest;
      Routes::Post(router, "/jobs", Routes::bind(&TestService::newJob, this));
      auto opts = Http::Endpoint::options().threads(static_cast<int>(thr));
      httpEndpoint->init(opts);
   }

   void start()
   {

      httpEndpoint->setHandler(router.handler());
      httpEndpoint->serve();
   }

private:
  

   void newJob(const Rest::Request& request, Http::ResponseWriter response)
   {
      
      if(scheduler.create(request.body()))
      {
         response.send(Http::Code::Ok, "Job created");
      }
   }

   std::shared_ptr<Http::Endpoint> httpEndpoint;
   Rest::Router router;
   JobScheduler scheduler{4};
};

int main(int argc, char* argv[])
{
   Port port(9080);

   int thr = 2;

   if (argc >= 2)
   {
      port = static_cast<uint16_t>(std::stol(argv[1]));

      if (argc == 3)
         thr = std::stoi(argv[2]);
   }

   Address addr(Ipv4::any(), port);

   cout << "Cores = " << hardware_concurrency() << endl;
   cout << "Using " << thr << " threads" << endl;

   TestService service(addr);

   service.init(thr);
   service.start();
}