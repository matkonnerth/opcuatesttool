#include "Comm.h"
#include "Client.h"
#include <cassert>
#include <chrono>
#include <thread>
#include <iostream>

namespace opctest::client {

Comm::Comm()
{   
}

void Comm::run()
{
    auto start = std::chrono::steady_clock::now();
    
    for(auto&client : clients)
    {
        //we could reschedule if a client is disconnected
        client->doComm();
    }
    auto end = std::chrono::steady_clock::now();
    auto delay = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    //std::cout << "doComm ms: " << delay.count() << std::endl;
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(20ms - delay);
}

std::shared_ptr<Client> Comm::createClient(const std::string& endpointUri)
{
    auto c = std::make_shared<Client>(endpointUri);
    clients.emplace_back(c);
    return c;
}

Comm::~Comm()
{
    for(auto c : clients)
    {
        c->disconnect();
    }    
}

} // namespace opctest::client