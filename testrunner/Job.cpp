#include "Job.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <modernOpc/OpcException.h>
#include <nlohmann/json.hpp>


namespace opctest::testrunner {

void Job::execute()
{
   Runtime rt{ m_serverUri, m_scriptDir, m_script };

   try
   {
      rt.load();

      auto steady_start = std::chrono::steady_clock::now();
      start = std::chrono::system_clock::now();
      m_status = JobStatus::RUNNING;

      rt.eval();


      auto steady_stop = std::chrono::steady_clock::now();
      stop = std::chrono::system_clock::now();
      std::chrono::duration<double, std::milli> duration;
      duration = steady_stop - steady_start;
      totalRuntime_ms = duration.count();
      m_status = JobStatus::FINISHED;
   }
   catch (modernopc::OpcException& e)
   {
      m_logger->error("OpcException during execution of job: {0}", e.what());
      m_status = JobStatus::ABORTED;
   }
   catch (std::exception& e)
   {
      m_logger->error("Script runtime exception: {0}", e.what());
      m_status = JobStatus::ABORTED;
   }
}

const std::string& Job::getServerUri() const
{
   return m_serverUri;
}

void Job::addResult(const std::string& inputFile, const std::string& outputFile, int id)
{
   using nlohmann::json;
   std::ifstream ifs1{ inputFile };

   auto j = json::parse(ifs1);

   auto result = json{ { "id", id },
                       {
                       "ts_start",
                       start.time_since_epoch().count(),
                       },
                       { "ts_stop", stop.time_since_epoch().count() },
                       { "totalRuntime_ms", totalRuntime_ms } };
   if (m_status != JobStatus::FINISHED)
   {
      result["statusCode"] = "Error";
   }
   else
   {
      result["statusCode"] = "Ok";
   }

   nlohmann::json output;
   output["result"] = result;
   output["request"] = j;

   std::ofstream out(outputFile);
   out << output;
   out.close();
}
} // namespace opctest::testrunner
