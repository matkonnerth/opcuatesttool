#include "Job.h"
#include "Task.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include "Client.h"


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
   if(client->getConnectionState()!=Client::ConnectionState::CONNECTED)
   {
      std::cout << "could not connect to server, abort job" << std::endl;
      status = JobStatus::ABORTED;
      return;
   }
   for (const auto& t : tasks)
   {
      if(!t->prepare(client))
      {
         status = JobStatus::ABORTED;
         std::cout << "prepare failed, abort job" << std::endl;
         return;
      }
   }
   auto start = std::chrono::steady_clock::now();
   status = JobStatus::RUNNING;
   for (auto i = 0u; i < iterations; i++)
   {
      for (const auto& t : tasks)
      {
         if(!t->execute(client))
         {
            status = JobStatus::ABORTED;
            std::cout << "execute failed, abort job" << std::endl;
            return;
         }
      }
   }
   status = JobStatus::FINISHED;
   auto end = std::chrono::steady_clock::now();
   std::chrono::duration<double, std::milli> duration;
   duration = end - start;
   totalRuntime_ms = duration.count();
   runtimePerIteration_ms = totalRuntime_ms / static_cast<double>(iterations);
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

   auto result = json{ { "totalRuntime_ms", totalRuntime_ms }, { "runtimePerIteration_ms", runtimePerIteration_ms } };
   if(status!= JobStatus::FINISHED)
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
