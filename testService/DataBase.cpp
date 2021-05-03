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

std::string DataBase::getScripts() const
{
   std::stringstream stream;
   stream << "[\n";
   bool first = true;
   for (auto& p : fs::directory_iterator(scriptDir))
   {
      if(first)
      {
         stream << "{\n";
         first=false;
      }
      else
      {
         stream << ",{\n";
      }
      stream << "\"name\": ";
      stream << p.path().stem();
      stream << "}";
   }
   stream << "]\n";
   auto debugString = stream.str();
   return debugString;
}

std::string DataBase::getScript(const std::string& name) const
{
   std::ifstream script(scriptDir + "/" + name + ".chai");
   if (script.fail())
   {
      //auto logger = spdlog::get("TestService");
      //logger->warn("job not found");
      return "job not found";
   }
   std::stringstream buffer;
   buffer << script.rdbuf();
   script.close();
   return buffer.str();
}

void DataBase::updateScript(const std::string&name, const std::string& content)
{
   std::ofstream out(scriptDir + "/" + name + ".chai");
   out << content;
   out.close();
}