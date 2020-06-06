#include "Job.h"
#include "Task.h"
#include <chrono>
#include <iostream>

namespace tt
{

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
    for(const auto& t:tasks)
    {
        t->prepare(client);
    }
    auto start = std::chrono::steady_clock::now();
    status = JobStatus::RUNNING;
    for(auto i =0u; i<iterations; i++)
    {
        for(const auto& t:tasks)
        {
            t->execute(client);
        }
    }
    status = JobStatus::FINISHED;
    auto end = std::chrono::steady_clock::now();
    totalRuntime_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
    runtimePerIteration_ms = totalRuntime_ms/iterations;
    std::cout << "Job finished" << "\n";
    std::cout << "Total runtime: " << totalRuntime_ms << "ms\n";
    std::cout << "runtime per iteration: " << runtimePerIteration_ms << "ms\n";
}

const std::string& Job::getServerUri() const
{
    return serverUri;
}
}
