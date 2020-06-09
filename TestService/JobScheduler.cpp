#include "JobScheduler.h"
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sys/types.h>
#include <unistd.h>

namespace fs = std::filesystem;

JobScheduler::JobScheduler(int maxConcurrentJobs)
: maxConcurrentJobs{ maxConcurrentJobs }
{}

bool JobScheduler::create(const std::string& jsonString)
{
   std::lock_guard<std::mutex> guard(_m);
   pid_t pid;
   int ret = 1;
   int status;

   std::string newRequest = std::to_string(jobId);
   std::string newRequestPath = requestedJobsDir + "/" + newRequest;

   std::ofstream out(newRequestPath);
   out << jsonString;
   out.close();

   pid = fork();

   if (pid == -1)
   {
      // pid == -1 means error occured
      printf("can't fork, error occured\n");
      exit(EXIT_FAILURE);
   }
   else if (pid == 0)
   {
      // pid == 0 means child process created
      // getpid() returns process id of calling process
      // Here It will return process id of child process

      // the argv list first argument should point to
      // filename associated with file being executed
      // the array pointer must be terminated by NULL
      // pointer
      char* argv_list[] = { "./testRunner", "/home/matzy/git/opcuaTestTool/build/bin", const_cast<char*>(finishedJobsDir.c_str()), const_cast<char*>(requestedJobsDir.c_str()), const_cast<char*>(newRequest.c_str()), NULL };

      // the execv() only return if error occured.
      // The return value is -1
      execv("./testRunner", argv_list);
      exit(0);
   }
   else
   {
      // parent
      printf("testrunner forked, pid = %u\n", pid);
      activeJobs[pid] = jobId;
   }
   jobId++;
   return true;
}

std::string JobScheduler::getFinishedJobs()
{
   for (auto& p : fs::directory_iterator(finishedJobsDir))
   {
      std::cout << p << "\n";
   }
   return "test";
}

void JobScheduler::jobFinished(int id)
{
   auto entry = activeJobs.find(id);
   if (entry != activeJobs.end())
   {
      std::cout << "job (pid: " << id << ", jobId: " << entry->second << ") finished\n";
   }
   else
   {
      std::cout << "received signal with pid " << id << " but no job active with this id?!\n";
   }
}

std::string JobScheduler::getFinishedJob(int jobId)
{
   std::ifstream job(finishedJobsDir + "/" + std::to_string(jobId));
   if (job.fail())
   {
      return "job not found";
   }
   std::stringstream buffer;
   buffer << job.rdbuf();
   job.close();
   return buffer.str();
}