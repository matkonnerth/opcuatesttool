#include "DataBase.h"
#include <fstream>


int DataBase::newJob(const std::string& requestJson)
{
   std::string request = requestJson;
   auto pos = request.find('{');
   int newJobId = ++jobId;
   std::string idString = "\"id\": " + std::to_string(newJobId) + ",";
   request.insert(pos + 1, idString);
   std::string newRequest = std::to_string(newJobId);
   std::string newRequestPath = getJobs_requests_dir() + "/" + newRequest;

   std::ofstream out(newRequestPath);
   out << request;
   out.close();
   return newJobId;
}
