#pragma once
#include "Task.h"
#include <memory>
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

enum class JobResult
{
   ERROR,
   OK
};

class Client;
class Job
{
public:
   explicit Job(const std::string& name, const std::string& serverUri)
   : name{ name }
   , serverUri{ serverUri }
   {}
   const std::string& getServerUri() const;
   void addTask(std::unique_ptr<Task> task);
   const std::vector<std::unique_ptr<Task>>& getTasks();
   virtual void execute(Client* client) = 0;
   virtual ~Job() = default;
   void addResult(const std::string& inputFile, const std::string& outputFile);

protected:
   JobStatus status{ JobStatus::INIT };
   JobResult resultCode{ JobResult::ERROR};
   const std::string name;
   std::vector<std::unique_ptr<Task>> tasks{};
   const std::string serverUri{};
   double totalRuntime_ms{ 0 };
   double runtimePerIteration_ms{ 0 };
};

class RepetiveJob : public Job
{
public:
   RepetiveJob(const std::string& name, const std::string& serverUri, size_t reps)
   : Job{ name, serverUri }
   , iterations{ reps }
   {}
   void execute(Client* client) override;

private:
   const size_t iterations{ 0 };
};

} // namespace tt
