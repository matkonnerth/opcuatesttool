#pragma once
#include "Config.h"
#include "DataBase.h"
#include <functional>
#include <mutex>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

namespace opctest {
class JobScheduler
{
public:
   JobScheduler(const std::string& workingDir, const std::string& repo);
   int create(const std::string& jsonString);
   void jobFinished(int pid);
   std::string getFinishedJobs(int fromId, int max);
   std::string getFinishedJob(int jobId);
   std::string getScripts() const;
   std::string getScript(const std::string& name) const;
   void updateScript(const std::string& name, const std::string& content);
   std::string getJobLog(int jobId);
   std::string getTargets() const;
   void setJobFinishedCallback(std::function<void(int)> cb)
   {
      m_fJobFinished = cb;
   }

private:
   void schedule();
   const std::string m_workingDir;
   const size_t maxConcurrentJobs{ 4 };
   std::mutex _m;
   std::unordered_map<int, int> activeJobs{};
   std::vector<int> finishedPids{};
   std::unique_ptr<DataBase> db{ nullptr };
   std::queue<int> waitQueue{};
   std::function<void(int)> m_fJobFinished{ nullptr };
};
} // namespace opctest
