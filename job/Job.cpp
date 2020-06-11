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

void RepetiveJob::execute(Client* client)
{
   if(client->getConnectionState()!=Client::ConnectionState::CONNECTED)
   {
      std::cout << "could not connect to server, abort job" << std::endl;
      status = JobStatus::ABORTED;
      return;
   }
   for (const auto& t : tasks)
   {
      t->prepare(client);
   }
   auto start = std::chrono::steady_clock::now();
   status = JobStatus::RUNNING;
   for (auto i = 0u; i < iterations; i++)
   {
      for (const auto& t : tasks)
      {
         t->execute(client);
      }
   }
   status = JobStatus::FINISHED;
   auto end = std::chrono::steady_clock::now();
   totalRuntime_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
   runtimePerIteration_ms = totalRuntime_ms / iterations;
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
   

   j["result"] = result;

   std::ofstream out(outputFile);
   out << j;
   out.close();
}
} // namespace tt
