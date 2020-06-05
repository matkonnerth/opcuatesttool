#pragma once
#include <string>
#include "NodeId.h"

namespace tt
{
class Client;
class Task
{
public:
    explicit Task(const std::string&name):name{name}{}
    virtual void prepare(Client* client) = 0;
    virtual void execute(Client* client) = 0;
    virtual ~Task()=default;
private:
    const std::string name;
};

class ReadRequest : public Task
{
public:
    ReadRequest(const std::string& name, const NodeId& idToRead): Task{name}, id{idToRead}{}
    void prepare(Client* client) override;
    void execute(Client* client) override;
private:
    NodeId id;
};
}
