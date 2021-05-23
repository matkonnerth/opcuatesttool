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
   DataBase(const std::string& root, const std::string& repo)
   : m_rootDir{ root }
   , m_jobs_requests_dir{ m_rootDir + "/jobs/requests" }
   , m_jobs_finished_dir{ m_rootDir + "/jobs/finished" }
   , m_scriptDir{ m_rootDir + "/repo/scripts" }
   , m_targetDir{ m_rootDir + "/repo/targets" }
   , m_repoUrl{ repo }
   {
      if (!fs::exists(m_rootDir + "/jobs"))
      {
         std::cout << "DB: jobs directory missing, initialize new\n";
         fs::create_directories(m_jobs_requests_dir);
         fs::create_directories(m_jobs_finished_dir);
      }
      else
      {
         // get the jobId
         int lastId = 0;
         while (fs::exists(m_jobs_requests_dir + "/" + std::to_string(lastId)))
         {
            lastId++;
         }
         m_jobId = lastId;
      }
      // load scripts from git repo
      int status = system((m_rootDir + "/getScripts.sh " + m_rootDir + "/repo " + m_repoUrl).c_str());
      if (status != 0)
      {
         auto logger = spdlog::get("TestService");
         logger->warn("could initialize script repo");
      }
   }

   int newJob(const std::string& requestJson);
   const std::string& getJobs_requests_dir() const
   {
      return m_jobs_requests_dir;
   }

   const std::string& getJobs_finished_dir() const
   {
      return m_jobs_finished_dir;
   }

   std::string getRequestFilePath(int id) const
   {
      return m_jobs_requests_dir + "/" + std::to_string(id);
   }

   std::string getFinishedFilePath(int id) const
   {
      return m_jobs_finished_dir + "/" + std::to_string(id);
   }

   std::string getJobLogPath(int id) const
   {
      return m_rootDir + "/logs/" + std::to_string(id) + ".log";
   }

   const std::string& getScriptsFilePath() const
   {
      return m_scriptDir;
   }

   std::string getScripts() const;
   std::string getScript(const std::string& name) const;
   void updateScript(const std::string& name, const std::string& content);
   std::string getFinishedJobs(int fromId, int max) const;
   std::string getFinishedJob(int jobId) const;
   std::string getJobLog(int jobId);
   std::string getTargets() const;

private:
   const std::string m_rootDir;
   const std::string m_jobs_requests_dir;
   const std::string m_jobs_finished_dir;
   const std::string m_scriptDir;
   const std::string m_targetDir;
   const std::string m_repoUrl;
   int m_jobId{ 0 };
};
} // namespace opctest
