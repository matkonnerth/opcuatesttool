#pragma once
#include <string>
#include <atomic>
#include <mutex>

class JobScheduler
{
public:
    explicit JobScheduler(int maxConcurrentJobs=4);
    bool create(const std::string& jsonString);
    std::string getFinishedJobs();
private:
    int maxConcurrentJobs {4};
    std::atomic<int> jobCount{0};
    std::mutex _m;
    const std::string requestedJobsDir {"./requests"};
    const std::string finishedJobsDir {"./finished"};
};
