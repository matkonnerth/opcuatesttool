#pragma once
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>
#include "DataBase.h"
#include <queue>

class JobScheduler
{
public:
   JobScheduler(const std::string& workingDir, int maxConcurrentJobs = 4);
   int create(const std::string& jsonString);
   void jobFinished(int pid);
   std::string getFinishedJobs(int fromId, int max);
   std::string getFinishedJob(int jobId);

private:
   void schedule();
   int maxConcurrentJobs{ 4 };
   std::mutex _m;
   const std::string workingDir;
   std::unordered_map<int, int> activeJobs{};
   std::vector<int> finishedPids{};
   std::unique_ptr<DataBase> db {nullptr};
   std::queue<int> waitQueue
   {};
};
