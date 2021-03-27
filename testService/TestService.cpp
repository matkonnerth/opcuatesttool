#include "../api/Request.h"
#include "../api/Server.h"
#include "JobScheduler.h"
#include <future>
#include <iostream>
#include <signal.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <type_traits>
#include <unistd.h>

namespace opctest::service {

using opctest::api::GetJobsRequest;
using opctest::api::GetJobRequest;
using opctest::api::GetJobResponse;
using opctest::api::NewJobRequest;
using opctest::api::NewJobResponse;

class TestService
{
public:
   TestService(const std::string& workingDir);

   void handleSigChld(int sig)
   {
      pid_t pid = wait(nullptr);
      scheduler->jobFinished(pid);
   }

   NewJobResponse newJob(const NewJobRequest& req)
   {

      auto id = scheduler->create(req.jsonRequest);
      NewJobResponse resp{};
      resp.ok = true;
      resp.id = id;
      auto logger = spdlog::get("testService");
      logger->info("newJob, id: {}", id);
      return resp;
   }

   GetJobResponse getJobs(const GetJobsRequest& req)
   {
      auto logger = spdlog::get("testService");

      GetJobResponse resp{};
      resp.ok = true;
      resp.data = scheduler->getFinishedJobs(req.from, req.max);
      return resp;
   }

   GetJobResponse getFinishedJob(const GetJobRequest& req)
   {
      GetJobResponse resp{};
      resp.ok = true;
      resp.data = scheduler->getFinishedJob(req.id);
      return resp;
   }

private:
   std::unique_ptr<JobScheduler> scheduler{ nullptr };
};

TestService::TestService(const std::string& workingDir)
: scheduler{ std::make_unique<JobScheduler>(workingDir) }
{
   auto logger = spdlog::get("testService");
   logger->info("init testService");
}

} // namespace opctest::service

void setupLogger(const std::string& workingDir)
{
   auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
   auto logger = std::make_shared<spdlog::logger>("testService", console_sink);
   logger->set_level(spdlog::level::debug);
   logger->info("logger init finished");
   spdlog::register_logger(logger);
}

template <class>
inline constexpr bool always_false_v = false;

bool apiCallback(opctest::service::TestService& service, const opctest::api::RequestVariant& req, opctest::api::ResponseVariant& resp)
{
   std::visit(
   [&](auto&& arg) {
      using T = std::decay_t<decltype(arg)>;
      if constexpr (std::is_same_v<T, opctest::api::GetJobRequest>)
      {
         resp = service.getFinishedJob(arg);
      }
      else if constexpr (std::is_same_v<T, opctest::api::GetJobsRequest>)
      {
         resp = service.getJobs(arg);
      }
      else if constexpr (std::is_same_v<T, opctest::api::NewJobRequest>)
      {
         resp = service.newJob(arg);
      }
      else
      {
         static_assert(always_false_v<T>, "non-exhaustive visitor!");
      }
   },
   req);

   return true;
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

   opctest::service::TestService service(binaryPath);

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

   opctest::api::Server server{ "0.0.0.0", 9888 };


   auto cb = [&](const opctest::api::RequestVariant& req, opctest::api::ResponseVariant & resp)
   {
      return apiCallback(service, req, resp);
   };

   server.setCallback(cb);
   server.listen();
}