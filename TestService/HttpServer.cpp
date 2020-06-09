#include "JobScheduler.h"
#include <pistache/description.h>
#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/router.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


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
   TestService(Address addr);

   void init(size_t thr = 1)
   {
      using namespace Rest;
      Routes::Post(router, "/jobs", Routes::bind(&TestService::newJob, this));
      Routes::Get(router, "/jobs/finished", Routes::bind(&TestService::getFinishedJobs, this));
      Routes::Get(router, "/jobs/finished/:jobId", Routes::bind(&TestService::getFinishedJob, this));
      auto opts = Http::Endpoint::options().threads(static_cast<int>(thr));
      httpEndpoint->init(opts);
   }

   void start()
   {

      httpEndpoint->setHandler(router.handler());
      httpEndpoint->serve();
   }

   static void handleSigChld(int sig)
   {
      pid_t pid = wait(nullptr);
      p_Scheduler->jobFinished(pid);
   }

private:
   void newJob(const Rest::Request& request, Http::ResponseWriter response)
   {
      // check for application/json content
      if (scheduler.create(request.body()))
      {
         response.send(Http::Code::Ok, "Job created");
      }
   }

   void getFinishedJobs(const Rest::Request& request, Http::ResponseWriter response)
   {
      response.send(Http::Code::Ok, scheduler.getFinishedJobs());
   }

   void getFinishedJob(const Rest::Request& request, Http::ResponseWriter response)
   {
      auto jobId = request.param(":jobId").as<int>();
      response.send(Http::Code::Ok, scheduler.getFinishedJob(jobId));
   }

   std::shared_ptr<Http::Endpoint> httpEndpoint;
   Rest::Router router;
   JobScheduler scheduler{ 4 };
   //need this ugly ptr for the signalHandler
   static JobScheduler* p_Scheduler;
};

JobScheduler* TestService::p_Scheduler {nullptr};

TestService::TestService(Address addr)
: httpEndpoint(std::make_shared<Http::Endpoint>(addr))
{
   p_Scheduler = &scheduler;
}

int main(int argc, char* argv[])
{
   signal(SIGCHLD, TestService::handleSigChld);
   Port port(9080);

   if (argc >= 2)
   {
      port = static_cast<uint16_t>(std::stol(argv[1]));
   }

   Address addr(Ipv4::any(), port);
   TestService service(addr);

   //epoll fails spontanously when using only 1 thread
   service.init(2);
   service.start();
}