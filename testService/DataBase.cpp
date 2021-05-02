#include "DataBase.h"
#include <fstream>


int DataBase::newJob(const std::string& requestJson)
{
   std::string request = requestJson;
   int newJobId = jobId;
   std::string newRequest = std::to_string(newJobId);
   std::string newRequestPath = getJobs_requests_dir() + "/" + newRequest;

   std::ofstream out(newRequestPath);
   out << requestJson;
   out.close();
   jobId++;
   return newJobId;
}
