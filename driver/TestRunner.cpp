#include "Client.h"
#include "Comm.h"
#include "Job.h"
#include "Task.h"
#include <iostream>
#include "JobFactory.h"


/*
    The testrunner instantiates exactly 1 client and executes exactly 1 job with this client.
    If jobs should run concurrently, multiple testrunner processes have to be instantiated.
*/
int main(int argc , const char** argv)
{
    for(int i=0; i<argc; i++)
    {
        std::cout << argv[i] << "\n";
    }
    if(argc!=3)
    {
       std::cout << "wrong number of arguments, path to job description needed\n";
       return 1;
    }

    tt::JobFactory f;
    auto job = f.createFromFile(argv[2]);

    tt::Comm comm;
    auto client = comm.createClient(job->getServerUri());
    client->connect();
    job->execute(client.get());
}