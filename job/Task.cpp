#include "Task.h"
#include "Client.h"
#include <chrono>
#include <thread>
#include "Job.h"

namespace tt {

bool ReadValue::prepare(TestClient* client)
{
   return client->cacheNodeId(id);
}

bool ReadValue::execute(const Job&, TestClient* client)
{
   auto r = client->read(id);
   result = std::make_unique<ReadValueResult>(std::move(r));
   return result->ok;
}

bool ReadValue::checkExpectation(const Expectation& exp) const
{
   return result->check(exp);
}

bool BrowseRequest::prepare(TestClient* client)
{
   return client->cacheNodeId(id);
}

bool BrowseRequest::execute(const Job&, TestClient* client)
{
   return client->browse(id);
}

bool GenericRequest::execute(const Job&, TestClient* client)
{
   return client->invokeGenericService(jsonRequest);
}

bool Wait::execute(const Job&, TestClient*)
{
   std::this_thread::sleep_for(std::chrono::milliseconds(delay));
   return true;
}

bool AssertValue::execute(const Job& j, TestClient*)
{
   return j.getPreviousTask().checkExpectation(exp);
}


} // namespace tt
