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

/* Jobs*/
std::unique_ptr<Job> createRepetiveJob(const std::string& name, const std::string& serverUri, const nlohmann::json& j)
{
   return std::make_unique<RepetiveJob>(name, serverUri, j.at("iterations").get<int>());
}

std::unique_ptr<Job> createOneCycleJob(const std::string& name, const std::string& serverUri, const nlohmann::json&)
{
   return std::make_unique<OneCycleJob>(name, serverUri);
}

const std::unordered_map<std::string, CreateJobFnc> JobFactory::jobs = { { "repetiveJob", createRepetiveJob }, { "oneCycleJob", createOneCycleJob } };
/* Tasks */
std::unique_ptr<Task> createReadRequestTask(const std::string& name, const nlohmann::json& j)
{
   auto id = NodeId(j["NodeId"]["ns"].get<std::string>(), j["NodeId"]["id"].get<std::string>());
   return std::make_unique<ReadRequest>(name, id);
}

std::unique_ptr<Task> createBrowseRequestTask(const std::string& name, const nlohmann::json& j)
{
   auto id = NodeId(j["NodeId"]["ns"].get<std::string>(), j["NodeId"]["id"].get<std::string>());
   return std::make_unique<BrowseRequest>(name, id);
}

std::unique_ptr<Task> createWaitTask(const std::string& name, const nlohmann::json& j)
{
   auto delay = j["delay_ms"].get<int>();
   return std::make_unique<Wait>(name, delay);
}

std::unique_ptr<Task> createGenericTask(const std::string& name, const nlohmann::json& j)
{
   auto request = j["serviceRequest"];
   return std::make_unique<GenericRequest>(name, request.dump());
}

const std::unordered_map<std::string, CreateTaskFnc> JobFactory::tasks = {
   { "readRequest", createReadRequestTask },
   { "browseRequest", createBrowseRequestTask },
   { "wait", createWaitTask},
   { "generic", createGenericTask}
};

std::unique_ptr<Job> JobFactory::createFromFile(const std::string& path)
{
   std::ifstream ifs1{ path };

   auto j = json::parse(ifs1);
   std::string type = j.at("type");

   auto name = j.at("name").get<std::string>();
   auto serverUri = j.at("serverUri").get<std::string>();

   std::unique_ptr<Job> job = jobs.at(type)(name, serverUri, j);

   // add tasks
   for (auto& el : j["tasks"].items())
   {
      auto type = el.value().at("type").get<std::string>();
      job->addTask(tasks.at(type)("task", el.value()));
   }
   return job;
}
} // namespace tt
