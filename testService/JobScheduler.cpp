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

JobScheduler::JobScheduler(const std::string& workingDir, int maxConcurrentJobs)
: workingDir{ workingDir }
, maxConcurrentJobs{ maxConcurrentJobs }
, db{ std::make_unique<DataBase>(workingDir) }
{}

int JobScheduler::create(const std::string& jsonString)
{
   std::lock_guard<std::mutex> guard(_m);
   pid_t pid;
   int ret = 1;
   int status;

   int newJobId = db->newJob(jsonString);

   pid = fork();

   if (pid == -1)
   {
      // pid == -1 means error occured
      printf("can't fork, error occured\n");
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
      char* argv_list[] = { "./testRunner", const_cast<char*>(workingDir.c_str()), const_cast<char*>(db->getJobs_finished_dir().c_str()), const_cast<char*>(db->getJobs_requests_dir().c_str()), const_cast<char*>(db->getRequestFilePath(newJobId).c_str()), NULL };

      // the execv() only return if error occured.
      // The return value is -1
      std::string testRunner = workingDir + "/testRunner";
      execv(testRunner.c_str(), argv_list);
      exit(0);
   }
   else
   {
      // parent
      printf("testrunner forked, pid = %u\n", pid);
      activeJobs[pid] = newJobId;
   }
   return newJobId;
}

void JobScheduler::getFinishedJobs(Http::ResponseStream& stream)
{
   using namespace Pistache;
   stream << "[\n";
   size_t cnt = 0;
   for (auto& p : fs::directory_iterator(db->getJobs_finished_dir()))
   {
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
   }
   stream << "]\n";
   stream.ends();
}

void JobScheduler::jobFinished(int id)
{
   std::lock_guard<std::mutex> guard(_m);
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
   std::ifstream job(db->getFinishedFilePath(jobId));
   if (job.fail())
   {
      return "job not found";
   }
   std::stringstream buffer;
   buffer << job.rdbuf();
   job.close();
   return buffer.str();
}