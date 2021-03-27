#include "Client.h"
#include "Comm.h"
#include "Job.h"
#include "JobFactory.h"
#include "Task.h"
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

using json = nlohmann::json;

/*
    The testrunner instantiates exactly 1 client and executes exactly 1 job with this client.
    If jobs should run concurrently, multiple testrunner processes have to be instantiated.
    finishedJobsDir argv[2]
    requestJobsDir argv[3]
    requestFilename argv[4]
*/

void setupLogger()
{
   auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
   auto logger = std::make_shared<spdlog::logger>("TestRunner", console_sink);
   logger->set_level(spdlog::level::debug);
   logger->info("logger init finished");
   spdlog::register_logger(logger);
}

int main(int argc , const char** argv)
{
    setupLogger();
    auto logger = spdlog::get("TestRunner");
    if(argc!=5)
    {
       logger->error("wrong number of arguments, exit");
       return 1;
    }

    std::string finishedDir = argv[2];
    std::string requestsDir = argv[3];
    std::string jobName = argv[4];

    tt::JobFactory f;
    auto job = f.createFromFile(requestsDir+"/"+jobName);

    tt::Comm comm;
    auto client = comm.createTestClient(job->getServerUri());
    client->connect();
    job->execute(client.get());
    job->addResult(requestsDir + "/" + jobName, finishedDir + "/" + jobName);
}