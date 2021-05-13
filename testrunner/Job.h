#pragma once
#include "Runtime.h"
#include <chrono>
#include <memory>
#include <spdlog/spdlog.h>
#include <string>
#include <vector>

namespace opctest::testrunner {

enum class JobStatus
{
   INIT,
   PENDING,
   RUNNING,
   FINISHED,
   ABORTED
};

class Job
{
public:
   explicit Job(const std::string& name, const std::string& serverUri, const std::string& scriptDir, const std::string& script)
   : name{ name }
   , serverUri{ serverUri }
   , logger{ spdlog::get("TestRunner") }
   , m_scriptDir{ scriptDir }
   , m_script{script}
   {}
   const std::string& getServerUri() const;
   virtual void execute();
   virtual ~Job() = default;
   void addResult(const std::string& inputFile, const std::string& outputFile, int id);

protected:
   JobStatus status{ JobStatus::INIT };
   const std::string name;
   const std::string serverUri{};
   double totalRuntime_ms{ 0 };
   //double runtimePerIteration_ms{ 0 };
   std::chrono::system_clock::time_point start{};
   std::chrono::system_clock::time_point stop{};
   std::shared_ptr<spdlog::logger> logger;
   std::string m_scriptDir;
   std::string m_script;
};


} // namespace opctest::testrunner
