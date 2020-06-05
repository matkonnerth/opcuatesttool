#include "Comm.h"
#include "Client.h"
#include "Job.h"
#include "Task.h"

int main(int, const char**)
{
    tt::Comm comm;
    auto client = comm.createClient("opc.tcp://192.168.110.10:4840");
    client->connect();

    auto rr = std::make_unique<tt::ReadRequest>("task1", tt::NodeId{"http://opcfoundation.org/UA/", "i=2255"});
    auto job = std::make_unique<tt::RepetiveJob>("job1", 100000000);

    job->addTask(std::move(rr));
    job->execute(client.get());

}