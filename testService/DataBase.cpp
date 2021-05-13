#include "DataBase.h"
#include <fstream>
#include <sstream>


namespace opctest {
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
      if (first)
      {
         stream << "{\n";
         first = false;
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
      auto logger = spdlog::get("TestService");
      logger->warn("job not found");
      return "job not found";
   }
   std::stringstream buffer;
   buffer << script.rdbuf();
   script.close();
   return buffer.str();
}

void DataBase::updateScript(const std::string& name, const std::string& content)
{
   std::ofstream out(scriptDir + "/" + name + ".chai");
   out << content;
   out.close();
}

std::string DataBase::getFinishedJobs(int fromId, int max) const
{
   std::stringstream stream;
   stream << "[\n";
   int cnt = 0;

   int highestId = -1;
   int fromFound = false;
   for (auto& p : fs::directory_iterator(getJobs_finished_dir()))
   {
      // find from
      try
      {
         int fileId = std::stoi(p.path().filename());
         if (highestId < fileId)
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

   if (fromFound)
   {
      for (int id = fromId; id < (fromId + max); id++)
      {
         if (cnt > 0)
         {
            stream << ",\n";
         }
         std::ifstream resultFile{ getJobs_finished_dir() + "/" + std::to_string(id) };
         if (resultFile.fail())
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
      // reverse
      for (int id = highestId; id >= 0 && id >= (highestId - max); id--)
      {
         std::ifstream resultFile{ getJobs_finished_dir() + "/" + std::to_string(id) };
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

std::string DataBase::getFinishedJob(int jobId) const
{
   std::ifstream job(getFinishedFilePath(jobId));
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

std::string DataBase::getJobLog(int jobId)
{
   std::ifstream log(getJobLogPath(jobId));
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

std::string DataBase::getTargets() const
{
   std::ifstream targets(targetDir + "/targets.json");
   if (targets.fail())
   {
      auto logger = spdlog::get("TestService");
      logger->warn("targets file not found");
      return "job log not found";
   }
   std::stringstream buffer;
   buffer << targets.rdbuf();
   targets.close();
   return buffer.str();
}
} // namespace opctest