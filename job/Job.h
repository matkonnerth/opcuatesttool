#pragma once
#include <string>
#include "Task.h"
#include <vector>
#include <memory>

namespace tt
{

enum class JobStatus
{
    INIT,
    PENDING,
    RUNNING,
    FINISHED
};

class Client;
class Job
{
public:
    explicit Job(const std::string&name):name{name}{}
    void addTask(std::unique_ptr<Task> task);
    const std::vector<std::unique_ptr<Task>>& getTasks();
    virtual void execute(Client* client)=0;
    virtual ~Job()=default;
protected:
    JobStatus status { JobStatus::INIT};
    const std::string name;
    std::vector<std::unique_ptr<Task>> tasks{};
};

class RepetiveJob : public Job
{
public:
    RepetiveJob(const std::string& name, size_t reps) : Job{name}, iterations{reps}{}
    size_t getIterations(){return iterations;}
    void execute(Client* client) override;
private:
    size_t iterations{0};
};

}
