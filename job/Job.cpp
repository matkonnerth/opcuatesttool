#include "Job.h"
#include "Client.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>



namespace tt {

void Job::execute()
{
   Runtime rt{serverUri, m_script};

   if(!rt.load())
   {
      status = JobStatus::ABORTED;
      return;
   }

   auto steady_start = std::chrono::steady_clock::now();
   start = std::chrono::system_clock::now();
   status = JobStatus::RUNNING;
   
   rt.eval();

   status = JobStatus::FINISHED;
   auto steady_stop = std::chrono::steady_clock::now();
   stop = std::chrono::system_clock::now();
   std::chrono::duration<double, std::milli> duration;
   duration = steady_stop - steady_start;
   totalRuntime_ms = duration.count();
   //runtimePerIteration_ms = totalRuntime_ms / static_cast<double>(iterations);
}

const std::string& Job::getServerUri() const
{
   return serverUri;
}

void Job::addResult(const std::string& inputFile, const std::string& outputFile)
{
   using nlohmann::json;
   std::ifstream ifs1{ inputFile };

   auto j = json::parse(ifs1);

   auto result = json{ {
                       "ts_start",
                       start.time_since_epoch().count(),
                       },
                       { "ts_stop", stop.time_since_epoch().count() },
                       { "totalRuntime_ms", totalRuntime_ms },
                       /*{ "runtimePerIteration_ms", runtimePerIteration_ms }*/ };
   if (status != JobStatus::FINISHED)
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
} // namespace tt
