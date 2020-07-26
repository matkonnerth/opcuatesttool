#include "JobScheduler.h"
#include <future>
#include <pistache/description.h>
#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/router.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

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
   TestService(Address addr, const std::string& workingDir);

   void init(size_t thr = 1)
   {
      using namespace Rest;
      Routes::Post(router, "/jobs", Routes::bind(&TestService::newJob, this));
      Routes::Get(router, "/jobs", Routes::bind(&TestService::getJobs, this));
      Routes::Get(router, "/jobs/:jobId", Routes::bind(&TestService::getFinishedJob, this));
      auto opts = Http::Endpoint::options().threads(static_cast<int>(thr));
      httpEndpoint->init(opts);
   }

   void start()
   {

      httpEndpoint->setHandler(router.handler());
      httpEndpoint->serve();
   }

   void handleSigChld(int sig)
   {
      pid_t pid = wait(nullptr);
      scheduler->jobFinished(pid);
   }

private:
   void newJob(const Rest::Request& request, Http::ResponseWriter response)
   {
      int id = scheduler->create(request.body());
      response.headers().add<Http::Header::ContentType>(MIME(Application, Json));
      response.send(Http::Code::Ok, "{\"StatusCode\": \"OK\", \"id\": " + std::to_string(id) + "}");
      auto logger = spdlog::get("testService");
      logger->info("newJob, id: {}", id);
   }

   void getJobs(const Rest::Request& request, Http::ResponseWriter response)
   {
      auto logger = spdlog::get("testService");
      auto query = request.query();
      if (query.has("finished"))
      {
         response.headers().add<Http::Header::ContentType>(MIME(Application, Json));
         auto stream = response.stream(Http::Code::Ok);
         int id=0;
         if(query.has("from"))
         {
            auto idString = query.get("from").get();
            try
            {
               id = std::stoi(idString);
            }
            catch(const std::exception& e)
            {
               std::cerr << e.what() << '\n';
               id=0;
            }
         }
         logger->info("getJobs, from id {}", id);
         scheduler->getFinishedJobs(stream, id);
      }
      else
      {
         response.send(Http::Code::Bad_Request, "wrong query");
         logger->warn("getJobs, unknown query");
      }
   }

   void getFinishedJob(const Rest::Request& request, Http::ResponseWriter response)
   {
      auto jobId = request.param(":jobId").as<int>();
      response.send(Http::Code::Ok, scheduler->getFinishedJob(jobId), MIME(Application, Json));
   }

   std::shared_ptr<Http::Endpoint> httpEndpoint;
   Rest::Router router;
   std::unique_ptr<JobScheduler> scheduler{ nullptr };
};

TestService::TestService(Address addr, const std::string& workingDir)
: httpEndpoint(std::make_shared<Http::Endpoint>(addr)), scheduler{std::make_unique<JobScheduler>(workingDir)}
{
   auto logger = spdlog::get("testService");
   logger->info("init http server, host: {}, port: {}", addr.host(), addr.port());
}

void setupLogger(const std::string& workingDir)
{
   auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
   auto logger = std::make_shared<spdlog::logger>("testService", console_sink);
   logger->set_level(spdlog::level::debug);
   logger->info("logger init finished");
   spdlog::register_logger(logger);
}

int main(int argc, char* argv[])
{
   std::string binaryPath = argv[0];
   auto pos = binaryPath.find_last_of('/');
   binaryPath.erase(pos);
   setupLogger(binaryPath);
   // handle signals in a dedicated thread
   sigset_t sigset;
   sigemptyset(&sigset);
   sigaddset(&sigset, SIGCHLD);
   pthread_sigmask(SIG_BLOCK, &sigset, nullptr);

   Port port(9080);

   if (argc >= 2)
   {
      port = static_cast<uint16_t>(std::stol(argv[1]));
   }

   Address addr(Ipv4::any(), port);
   TestService service(addr, binaryPath);

   auto signalHandler = [&service, &sigset]() {
      while (true)
      {
         int signum = 0;
         // wait until a signal is delivered:
         sigwait(&sigset, &signum);
         service.handleSigChld(signum);
      }
   };

   auto ft_signal_handler = std::async(std::launch::async, signalHandler);
   service.init(2);
   service.start();
}