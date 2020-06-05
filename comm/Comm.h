#pragma once
#include <string>
#include <vector>
#include <memory>

namespace tt
{
class Client;
class Comm
{
public:
    Comm();
    ~Comm();
    Comm(const Comm&) = delete;
    Comm& operator=(const Comm&) = delete;
    void run();
    std::shared_ptr<Client> createClient(const std::string& endpointUri);

private:
    std::vector<std::shared_ptr<Client>> clients{};
};
}