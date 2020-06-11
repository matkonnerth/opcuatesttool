#include "Client.h"
#include "Comm.h"
#include "Job.h"
#include "JobFactory.h"
#include "Task.h"
#include <iostream>


/*
    The testrunner instantiates exactly 1 client and executes exactly 1 job with this client.
    If jobs should run concurrently, multiple testrunner processes have to be instantiated.
    finishedJobsDir argv[2]
    requestJobsDir argv[3]
    filename argv[4]
    
*/
int main(int argc , const char** argv)
{
    for(int i=0; i<argc; i++)
    {
        std::cout << argv[i] << "\n";
    }
    if(argc!=5)
    {
       std::cout << "wrong number of arguments, path to job description needed\n";
       return 1;
    }

    std::string jobName = argv[4];
    std::string requestsDir = argv[3];
    std::string finishedDir = argv[2];

    tt::JobFactory f;    
    auto job = f.createFromFile(requestsDir+"/"+jobName);

    tt::Comm comm;
    auto client = comm.createClient(job->getServerUri());
    client->connect();
    job->execute(client.get());
    job->addResult(requestsDir + "/" + jobName, finishedDir + "/" + jobName);
}