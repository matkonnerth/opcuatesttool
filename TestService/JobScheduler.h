#pragma once
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

class JobScheduler
{
public:
   explicit JobScheduler(int maxConcurrentJobs = 4);
   bool create(const std::string& jsonString);
   void jobFinished(int pid);
   std::string getFinishedJobs();
   std::string getFinishedJob(int jobId);

private:
   int maxConcurrentJobs{ 4 };
   int jobId{ 0 };
   std::mutex _m;
   const std::string requestedJobsDir{ "./requests" };
   const std::string finishedJobsDir{ "./finished" };
   std::unordered_map<int, int> activeJobs{};
   std::vector<int> finishedPids{};
};
