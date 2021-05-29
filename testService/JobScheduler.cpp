#include "JobScheduler.h"
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <spdlog/spdlog.h>
#include <sstream>
#include <sys/types.h>
#include <unistd.h>

namespace fs = std::filesystem;

namespace opctest {
JobScheduler::JobScheduler(const std::string& workingDir, const std::string& repo)
: m_workingDir{ workingDir }
, db{ std::make_unique<DataBase>(m_workingDir, repo) }
{}

void JobScheduler::schedule()
{
   if (waitQueue.empty())
   {
      return;
   }
   auto logger = spdlog::get("TestService");
   if (activeJobs.size() >= maxConcurrentJobs)
   {
      logger->trace("too much active jobs, postpone scheduling, active jobs {}", activeJobs.size());
      return;
   }
   int jobId = waitQueue.front();
   waitQueue.pop();
   int pid = fork();

   if (pid == -1)
   {
      // pid == -1 means error occured

      logger->error("failed to fork TestRunner, exit");
      exit(EXIT_FAILURE);
   }
   else if (pid == 0)
   {
      // child process
      // close file descriptors, let stdin/stdout/stderror open
      for (int fd = 5; fd < 256; fd++)
      {
         close(fd);
      }

      auto file = fopen((m_workingDir + "/logs/" + std::to_string(jobId) + ".log").c_str(), "w");
      if (file == NULL)
         fputs("Could not open file", stderr);

      dup2(fileno(file), STDOUT_FILENO);
      fclose(file);

      // the argv list first argument should point to
      // filename associated with file being executed
      // the array pointer must be terminated by NULL
      // pointer

      // the const casts tare ugly but execv isn't modifying
      std::string jobIdString = std::to_string(jobId);
      char* const argv_list[] = { "./testRunner",
                                  const_cast<char*>(m_workingDir.c_str()),
                                  const_cast<char*>(db->getJobs_finished_dir().c_str()),
                                  const_cast<char*>(db->getJobs_requests_dir().c_str()),
                                  const_cast<char*>(jobIdString.c_str()),
                                  const_cast<char*>(db->getScriptsFilePath().c_str()),
                                  nullptr };

      // the execv() only return if error occured.
      // The return value is -1
      std::string testRunner = m_workingDir + "/testRunner";
      execv(testRunner.c_str(), argv_list);
      exit(0);
   }
   else
   {
      // parent
      logger->info("testrunner forked, pid {}", pid);
      activeJobs[pid] = jobId;
   }
}

int JobScheduler::create(const std::string& jsonString)
{
   std::lock_guard<std::mutex> guard(_m);
   int newJobId = db->newJob(jsonString);
   waitQueue.push(newJobId);
   schedule();
   return newJobId;
}

void JobScheduler::jobFinished(int id)
{
   auto logger = spdlog::get("TestService");
   std::lock_guard<std::mutex> guard(_m);
   auto entry = activeJobs.find(id);
   if (entry != activeJobs.end())
   {
      logger->info("job (pid: {}, id: {}) finished", id, entry->second);
      if (m_fJobFinished)
      {
         m_fJobFinished("JobFinishedEvent", std::to_string(entry->second));
      }
      activeJobs.erase(entry);
   }
   else
   {
      logger->warn("received signal with pid {} but no job active with this id?!", id);
   }
   schedule();
}

std::string JobScheduler::getFinishedJobs(int fromId, int max)
{
   return db->getFinishedJobs(fromId, max);
}

std::string JobScheduler::getFinishedJob(int jobId)
{
   return db->getFinishedJob(jobId);
}

std::string JobScheduler::getScripts() const
{
   return db->getScripts();
}

std::string JobScheduler::getScript(const std::string& name) const
{
   return db->getScript(name);
}

void JobScheduler::updateScript(const std::string& name, const std::string& content)
{
   db->updateScript(name, content);
}

std::string JobScheduler::getJobLog(int jobId)
{
   return db->getJobLog(jobId);
}

std::string JobScheduler::getTargets() const
{
   return db->getTargets();
}
} // namespace opctest