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
    virtual bool prepare(Client* client) = 0;
    virtual bool execute(Client* client) = 0;
    virtual ~Task()=default;
private:
    const std::string name;
};

class ReadRequest : public Task
{
public:
    ReadRequest(const std::string& name, const NodeId& idToRead): Task{name}, id{idToRead}{}
    bool prepare(Client* client) override;
    bool execute(Client* client) override;
private:
    NodeId id;
};

class Wait : public Task
{
public:
   Wait(const std::string& name, int milliSeconds)
   : Task{ name }
   , delay{ milliSeconds}
   {}
   bool prepare(Client* client) override;
   bool execute(Client* client) override;

private:
   int delay {0};
};
}
