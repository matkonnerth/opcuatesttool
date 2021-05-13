#pragma once
#include "Config.h"
#include <filesystem>
#include <iostream>
#include <spdlog/spdlog.h>
#include <string>

namespace fs = std::filesystem;

/*

root
    -> jobs
            -> requests
            -> finished
            -> logs
    -> scripts (this is the script repo)
         -> scripts (directory with scripts)

*/

namespace opctest {
class DataBase
{
public:
   DataBase(const std::string& root, const Config& c)
   : rootDir{ root }
   , jobs_requests_dir{ rootDir + "/jobs/requests" }
   , jobs_finished_dir{ rootDir + "/jobs/finished" }
   , scriptDir{ rootDir + "/repo/scripts" }
   , targetDir{ rootDir + "/repo/targets" }
   , config{ c }
   {
      if (!fs::exists(rootDir + "/jobs"))
      {
         std::cout << "DB: jobs directory missing, initialize new\n";
         fs::create_directories(jobs_requests_dir);
         fs::create_directories(jobs_finished_dir);
      }
      else
      {
         // get the jobId
         int lastId = 0;
         while (fs::exists(jobs_requests_dir + "/" + std::to_string(lastId)))
         {
            lastId++;
         }
         jobId = lastId;
      }
      // load scripts from git repo
      int status = system((rootDir + "/getScripts.sh " + rootDir + "/repo " + config.gitRepository).c_str());
      if (status != 0)
      {
         auto logger = spdlog::get("TestService");
         logger->warn("could initialize script repo");
      }
   }

   int newJob(const std::string& requestJson);
   const std::string& getJobs_requests_dir() const
   {
      return jobs_requests_dir;
   }

   const std::string& getJobs_finished_dir() const
   {
      return jobs_finished_dir;
   }

   std::string getRequestFilePath(int id) const
   {
      return jobs_requests_dir + "/" + std::to_string(id);
   }

   std::string getFinishedFilePath(int id) const
   {
      return jobs_finished_dir + "/" + std::to_string(id);
   }

   std::string getJobLogPath(int id) const
   {
      return rootDir + "/logs/" + std::to_string(id) + ".log";
   }

   const std::string& getScriptsFilePath() const
   {
      return scriptDir;
   }

   std::string getScripts() const;
   std::string getScript(const std::string& name) const;
   void updateScript(const std::string& name, const std::string& content);
   std::string getFinishedJobs(int fromId, int max) const;
   std::string getFinishedJob(int jobId) const;
   std::string getJobLog(int jobId);
   std::string getTargets() const;

private:
   const std::string rootDir;
   const std::string jobs_requests_dir;
   const std::string jobs_finished_dir;
   const std::string scriptDir;
   const std::string targetDir;
   int jobId{ 0 };
   const Config& config;
};
} // namespace opctest
