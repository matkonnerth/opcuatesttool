#include "JobScheduler.h"
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <spdlog/spdlog.h>
#include <sstream>
#include <sys/types.h>
#include <unistd.h>

namespace fs = std::filesystem;

JobScheduler::JobScheduler(const std::string& workingDir)
: workingDir{ workingDir }
, db{ std::make_unique<DataBase>(workingDir) }
{}

void JobScheduler::schedule()
{
   auto logger = spdlog::get("TestService");
   if (waitQueue.empty())
   {
      return;
   }
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
      for (int fd = 3; fd < 256; fd++)
      {
         close(fd);
      }

      // the argv list first argument should point to
      // filename associated with file being executed
      // the array pointer must be terminated by NULL
      // pointer

      // the const cass tare ugly but execv isn't modifying
      std::string jobIdString = std::to_string(jobId);
      char* const argv_list[] = { "./testRunner",
                                  const_cast<char*>(workingDir.c_str()),
                                  const_cast<char*>(db->getJobs_finished_dir().c_str()),
                                  const_cast<char*>(db->getJobs_requests_dir().c_str()),
                                  const_cast<char*>(jobIdString.c_str()),
                                  const_cast<char*>(db->getScriptsFilePath().c_str()),
                                  nullptr };

      // the execv() only return if error occured.
      // The return value is -1
      std::string testRunner = workingDir + "/testRunner";
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
   std::stringstream stream;
   stream << "[\n";
   int cnt = 0;
   for (auto& p : fs::directory_iterator(db->getJobs_finished_dir()))
   {
      try
      {
         int fileId = std::stoi(p.path().filename());
         if (fileId < fromId)
         {
            continue;
         }
      }
      catch (const std::exception& e)
      {
         std::cerr << e.what() << '\n';
      }

      if (cnt > 0)
      {
         stream << ",\n";
      }

      std::ifstream resultFile{ p.path() };
      std::string line;
      while (getline(resultFile, line))
      {
         stream << line.c_str() << '\n';
      }
      cnt++;
      if (cnt >= max)
      {
         break;
      }
   }
   stream << "]\n";
   return stream.str();
}

std::string JobScheduler::getFinishedJob(int jobId)
{
   std::ifstream job(db->getFinishedFilePath(jobId));
   if (job.fail())
   {
      auto logger = spdlog::get("TestService");
      logger->warn("job not found");
      return "job not found";
   }
   std::stringstream buffer;
   buffer << job.rdbuf();
   job.close();
   return buffer.str();
}