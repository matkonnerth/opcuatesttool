#pragma once
#include "Task.h"
#include <chrono>
#include <memory>
#include <spdlog/spdlog.h>
#include <string>
#include <vector>

namespace tt {

enum class JobStatus
{
   INIT,
   PENDING,
   RUNNING,
   FINISHED,
   ABORTED
};

class TestClient;
class Job
{
public:
   explicit Job(const std::string& name, const std::string& serverUri)
   : name{ name }
   , serverUri{ serverUri }, logger{spdlog::get("TestRunner")}
   {
   }
   const std::string& getServerUri() const;
   void addTask(std::unique_ptr<Task> task);
   const std::vector<std::unique_ptr<Task>>& getTasks();
   virtual void execute(TestClient* client) = 0;
   virtual ~Job() = default;
   void addResult(const std::string& inputFile, const std::string& outputFile);
   const Task& getPreviousTask() const;

protected:
   Task* previousTask{nullptr};
   JobStatus status{ JobStatus::INIT };
   const std::string name;
   std::vector<std::unique_ptr<Task>> tasks{};
   const std::string serverUri{};
   double totalRuntime_ms{ 0 };
   double runtimePerIteration_ms{ 0 };
   std::chrono::system_clock::time_point start{};
   std::chrono::system_clock::time_point stop{};
   std::shared_ptr<spdlog::logger> logger;
};

class RepetiveJob : public Job
{
public:
   RepetiveJob(const std::string& name, const std::string& serverUri, size_t reps)
   : Job{ name, serverUri }
   , iterations{ reps }
   {}
   void execute(TestClient* client) override;

private:
   const size_t iterations{ 0 };

};

class OneCycleJob : public RepetiveJob
{
public:
   OneCycleJob(const std::string& name, const std::string& serverUri)
   : RepetiveJob{ name, serverUri, 1 }
   {};
};

} // namespace tt
