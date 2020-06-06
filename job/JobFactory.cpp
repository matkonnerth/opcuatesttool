#include "JobFactory.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include "Task.h"
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
std::unique_ptr<Job> JobFactory::createFromFile(const std::string& path)
{
   std::ifstream ifs1{ path };

   auto j = json::parse(ifs1);
   std::string type = j.at("type");

   auto name = j.at("name").get<std::string>();
   auto serverUri = j.at("serverUri").get<std::string>();

   std::unique_ptr<Job> job;

   if (type == "repetiveJob")
   {
      job = std::make_unique<RepetiveJob>(name, serverUri, j.at("iterations").get<int>());
   }
   if (!job)
   {
      return job;
   }

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
   }


   return job;
}
} // namespace tt
