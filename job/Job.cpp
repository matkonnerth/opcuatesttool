#include "Job.h"
#include "Task.h"

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
    status = JobStatus::RUNNING;
    for(auto i =0u; i<iterations; i++)
    {
        for(const auto& t:tasks)
        {
            t->execute(client);
        }
    }
    status = JobStatus::FINISHED;
}

}
