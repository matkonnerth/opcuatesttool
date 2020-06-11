#include "JobFactory.h"
#include "Task.h"
#include <fstream>

using json = nlohmann::json;

/*
{
    "name": "job1",
    "serverUri": "opc.tcp://localhost:4840",
    "type": "repetiveJob",
    "iterations": 10000,
    "tasks": [
        {
            "type": "readRequest",
            "NodeId": {
                "ns": "http://opcfoundation.org/UA/",
                "id": "i=2255"
            }
        }
    ]
}
*/


namespace tt {

std::unique_ptr<Job> createRepetiveJob(const std::string& name, const std::string& serverUri, const nlohmann::json& j)
{
   return std::make_unique<RepetiveJob>(name, serverUri, j.at("iterations").get<int>());
}

std::unique_ptr<Job> createOneCycleJob(const std::string& name, const std::string& serverUri, const nlohmann::json&)
{
   return std::make_unique<OneCycleJob>(name, serverUri);
}


const std::unordered_map<std::string, CreateJobFnc> JobFactory::jobs = { { "repetiveJob", createRepetiveJob }, { "oneCycleJob", createOneCycleJob } };

std::unique_ptr<Job> JobFactory::createFromFile(const std::string& path)
{
   std::ifstream ifs1{ path };

   auto j = json::parse(ifs1);
   std::string type = j.at("type");

   auto name = j.at("name").get<std::string>();
   auto serverUri = j.at("serverUri").get<std::string>();

   std::unique_ptr<Job> job = jobs.at(type)(name, serverUri, j);

   // get the task
   for (auto& el : j["tasks"].items())
   {
      auto type = el.value().at("type").get<std::string>();
      if (type == "readRequest")
      {
         auto id = NodeId(el.value()["NodeId"]["ns"].get<std::string>(), el.value()["NodeId"]["id"].get<std::string>());
         auto task = std::make_unique<ReadRequest>("task", id);
         job->addTask(std::move(task));
      }
      else if (type == "wait")
      {
         auto delay = el.value()["delay_ms"].get<int>();
         auto task = std::make_unique<Wait>("task", delay);
         job->addTask(std::move(task));
      }
   }


   return job;
}
} // namespace tt
