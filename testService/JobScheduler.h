#pragma once
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>
#include "DataBase.h"
#include <queue>
#include <functional>

class JobScheduler
{
public:
   JobScheduler(const std::string& workingDir);
   int create(const std::string& jsonString);
   void jobFinished(int pid);
   std::string getFinishedJobs(int fromId, int max);
   std::string getFinishedJob(int jobId);
   std::string getScripts() const;
   std::string getScript(const std::string& name) const;
   void updateScript(const std::string&name, const std::string& content);
   std::string getJobLog(int jobId);
   void setJobFinishedCallback(std::function<void(int)> cb)
   {
      m_fJobFinished = cb;
   }

private:
   void schedule();
   const std::string workingDir;
   const size_t maxConcurrentJobs{ 4 };
   std::mutex _m;   
   std::unordered_map<int, int> activeJobs{};
   std::vector<int> finishedPids{};
   std::unique_ptr<DataBase> db {nullptr};
   std::queue<int> waitQueue
   {};
   std::function<void(int)> m_fJobFinished{ nullptr };
};
