#pragma once
#include <string>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

/*

root
    -> jobs
            -> requests
            -> finished
            -> logs
    -> scripts

*/

class DataBase
{
public:
   DataBase(const std::string& root)
   : rootDir{ root }, jobs_requests_dir{rootDir+"/jobs/requests"}, jobs_finished_dir{rootDir+"/jobs/finished"}
   {
       if(!fs::exists(rootDir+"/jobs"))
       {
           std::cout << "DB: jobs directory missing, initialize new\n";
           fs::create_directories(jobs_requests_dir);
           fs::create_directories(jobs_finished_dir);
       }
       else
       {
           //get the jobId
           int lastId=0;
           while(fs::exists(jobs_requests_dir+"/"+std::to_string(lastId)))
           {
               lastId++;
           }
           jobId=lastId;
       }
       if(!fs::exists(getScriptsFilePath()))
       {
          std::cout << "DB: scripts directory missing, initialize new\n";
          fs::create_directories(getScriptsFilePath());
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
       return  jobs_requests_dir + "/" + std::to_string(id);
   }

   std::string getFinishedFilePath(int id) const
   {
      return jobs_finished_dir + "/" + std::to_string(id);
   }

   std::string getScriptsFilePath() const
   {
       return rootDir + "/scripts";
   }

private:
    const std::string rootDir;
    const std::string jobs_requests_dir;
    const std::string jobs_finished_dir;
    int jobId{ 0 };
};