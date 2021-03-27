#pragma once
#include "Expectation.h"
#include "NodeId.h"
#include "Result.h"
#include <memory>
#include <string>
#include <stdexcept>

namespace tt
{
   class TestClient;
   class Job;
   class Task
   {
   public:
      explicit Task(const std::string& name)
      : name{ name }
      {}
      virtual bool prepare(TestClient*)
      {
         return true;
      }
      virtual bool execute(const Job& job, TestClient* client) = 0;
      virtual bool checkExpectation(const Expectation&) const
      {
         throw std::runtime_error("Expectation not supported by this task.");
      }
      virtual ~Task() = default;
      const std::string& getName() const
      {
         return name;
      }

   private:
      const std::string name;
   };

   class ReadValue : public Task
   {
   public:
      ReadValue(const std::string& name, const NodeId& idToRead)
      : Task{ name }
      , id{ idToRead }
      {}
      bool prepare(TestClient* client) override;
      bool execute(const Job& job, TestClient* client) override;
      bool checkExpectation(const Expectation& exp) const override;

   private:
      NodeId id;
      std::unique_ptr<ReadValueResult> result{ nullptr };
   };

   class BrowseRequest : public Task
   {
   public:
      BrowseRequest(const std::string& name, const NodeId& id)
      : Task{ name }
      , id{ id }
      {}
      bool prepare(TestClient* client) override;
      bool execute(const Job& job, TestClient* client) override;

   private:
      NodeId id;
   };

   class Wait : public Task
   {
   public:
      Wait(const std::string& name, int milliSeconds)
      : Task{ name }
      , delay{ milliSeconds }
      {}
      bool execute(const Job& job, TestClient* client) override;

   private:
      int delay{ 0 };
   };

   class GenericRequest : public Task
   {
   public:
      GenericRequest(const std::string& name, const std::string& jsonRequest)
      : Task{ name }
      , jsonRequest{ jsonRequest }
      {}
      bool execute(const Job& job, TestClient* client) override;

   private:
      const std::string jsonRequest;
   };

   class AssertValue : public Task
   {
   public:
      AssertValue(const std::string& name, const std::string& expectedValue)
      : Task(name)
      , exp{ expectedValue }
      {}

      bool execute(const Job& job, TestClient* client) override;

   private:
      const ValueExpectation exp;
   };
} // namespace tt
