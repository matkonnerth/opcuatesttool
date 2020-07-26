#include "Job.h"
#include "Client.h"
#include "Task.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>


namespace tt {

void Job::addTask(std::unique_ptr<Task> task)
{
   tasks.emplace_back(std::move(task));
}

const std::vector<std::unique_ptr<Task>>& Job::getTasks()
{
   return tasks;
}

void RepetiveJob::execute(TestClient* client)
{
   if (client->getConnectionState() != Client::ConnectionState::CONNECTED)
   {
      std::cout << "could not connect to server, abort job" << std::endl;
      status = JobStatus::ABORTED;
      return;
   }
   for (const auto& t : tasks)
   {
      if (!t->prepare(client))
      {
         status = JobStatus::ABORTED;
         std::cout << "prepare failed, abort job" << std::endl;
         return;
      }
   }
   auto steady_start = std::chrono::steady_clock::now();
   start = std::chrono::system_clock::now();
   status = JobStatus::RUNNING;
   for (auto i = 0u; i < iterations; i++)
   {
      previousTask=nullptr;
      for (const auto& t : tasks)
      {
         if (!t->execute(*this, client))
         {
            status = JobStatus::ABORTED;
            std::cout << "execute failed, abort job" << std::endl;
            return;
         }
         previousTask=t.get();
      }
   }
   status = JobStatus::FINISHED;
   auto steady_stop = std::chrono::steady_clock::now();
   stop = std::chrono::system_clock::now();
   std::chrono::duration<double, std::milli> duration;
   duration = steady_stop - steady_start;
   totalRuntime_ms = duration.count();
   runtimePerIteration_ms = totalRuntime_ms / static_cast<double>(iterations);
}

const Task& Job::getPreviousTask() const
{
   if(!previousTask)
   {
      throw std::runtime_error("requested previous Task, but there is no previous task");
   }
   return *previousTask;
}

const std::string& Job::getServerUri() const
{
   return serverUri;
}

void Job::addResult(const std::string& inputFile, const std::string& outputFile)
{
   using nlohmann::json;
   std::ifstream ifs1{ inputFile };

   auto j = json::parse(ifs1);

   auto result = json{ {
                       "ts_start",
                       start.time_since_epoch().count(),
                       },
                       { "ts_stop", stop.time_since_epoch().count() },
                       { "totalRuntime_ms", totalRuntime_ms },
                       { "runtimePerIteration_ms", runtimePerIteration_ms } };
   if (status != JobStatus::FINISHED)
   {
      result["statusCode"] = "Error";
   }
   else
   {
      result["statusCode"] = "Ok";
   }

   nlohmann::json output;
   output["result"] = result;
   output["request"] = j;

   std::ofstream out(outputFile);
   out << output;
   out.close();
}
} // namespace tt
