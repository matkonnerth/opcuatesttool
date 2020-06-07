#include "JobScheduler.h"
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sys/types.h>
#include <unistd.h>
#include <filesystem>
#include <iostream>

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

   std::string newRequest = std::to_string(jobCount);
   std::string newRequestPath = requestedJobsDir + "/" + newRequest;
   jobCount++;
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
      printf("child process, pid = %u\n", getpid());
      // Here It will return Parent of child Process means Parent process it self
      printf("parent of child process, pid = %u\n", getppid());

      // the argv list first argument should point to
      // filename associated with file being executed
      // the array pointer must be terminated by NULL
      // pointer
      char* argv_list[] = { "./testRunner", "/home/matzy/git/opcuaTestTool/build/bin", 
      const_cast<char*>(finishedJobsDir.c_str()), 
      const_cast<char*>(requestedJobsDir.c_str()),
      const_cast<char*>(newRequest.c_str()), NULL };

      // the execv() only return if error occured.
      // The return value is -1
      execv("./testRunner", argv_list);
      exit(0);
   }
   else
   {
      // parent
   }
   return true;
}

std::string JobScheduler::getFinishedJobs()
{
   for(auto& p:fs::directory_iterator(finishedJobsDir))
   {
      std::cout << p << "\n";
   }
   return "test";
}