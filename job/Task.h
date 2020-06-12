#pragma once
#include <string>
#include "NodeId.h"

namespace tt
{
class TestClient;
class Task
{
public:
    explicit Task(const std::string&name):name{name}{}
    virtual bool prepare(TestClient* client) = 0;
    virtual bool execute(TestClient* client) = 0;
    virtual ~Task()=default;
private:
    const std::string name;
};

class ReadRequest : public Task
{
public:
    ReadRequest(const std::string& name, const NodeId& idToRead): Task{name}, id{idToRead}{}
    bool prepare(TestClient* client) override;
    bool execute(TestClient* client) override;
private:
    NodeId id;
};

class BrowseRequest : public Task
{
public:
   BrowseRequest(const std::string& name, const NodeId& id)
   : Task{ name }
   , id{ id }
   {}
   bool prepare(TestClient* client) override;
   bool execute(TestClient* client) override;

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
   bool prepare(TestClient* client) override;
   bool execute(TestClient* client) override;

private:
   int delay {0};
};

class GenericRequest : public Task
{
public:
   GenericRequest(const std::string& name, const std::string& jsonRequest)
   : Task{ name }
   , jsonRequest{ jsonRequest }
   {}
   bool prepare(TestClient* client) override;
   bool execute(TestClient* client) override;

private:
   const std::string jsonRequest;
};
}
