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

JobScheduler::JobScheduler(const std::string& workingDir)
: workingDir{ workingDir }
, db{ std::make_unique<DataBase>(workingDir) }
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
      for (int fd = 0; fd < 256; fd++)
      {
         close(fd);
      }

      auto file = fopen((workingDir + "/logs/" + std::to_string(jobId) + ".log").c_str(), "w");
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
      if (m_fJobFinished)
      {
         m_fJobFinished(entry->second);
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
   std::stringstream stream;
   stream << "[\n";
   int cnt = 0;

   int highestId = -1;
   int fromFound = false;
   for (auto& p : fs::directory_iterator(db->getJobs_finished_dir()))
   {
      //find from
      try
      {
         int fileId = std::stoi(p.path().filename());
         if(highestId<fileId)
         {
            highestId = fileId;
         }
         if (fileId == fromId)
         {
            fromFound = true;
            break;
         }
      }
      catch (const std::exception& e)
      {
         std::cerr << e.what() << '\n';
      }
   }

   if(fromFound)
   {
      for(int id = fromId; id < (fromId+max); id++)
      {
         if (cnt > 0)
         {
            stream << ",\n";
         }
         std::ifstream resultFile{ db->getJobs_finished_dir() + "/" + std::to_string(id)};
         if(resultFile.fail())
         {
            break;
         }
         std::string line;
         while (getline(resultFile, line))
         {
            stream << line.c_str() << '\n';
         }
         cnt++;
      }
   }
   else
   {
      //reverse
      for(int id = highestId; id>=0 && id>=(highestId-max);id--)
      {
         std::ifstream resultFile{ db->getJobs_finished_dir() + "/" + std::to_string(id) };
         if (resultFile.fail())
         {
            break;
         }
         if (cnt > 0)
         {
            stream << ",\n";
         }
         std::string line;
         while (getline(resultFile, line))
         {
            stream << line.c_str() << '\n';
         }
         cnt++;
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
   std::ifstream log(db->getJobLogPath(jobId));
   if (log.fail())
   {
      auto logger = spdlog::get("TestService");
      logger->warn("job log not found");
      return "job log not found";
   }
   std::stringstream buffer;
   buffer << log.rdbuf();
   log.close();
   return buffer.str();
}