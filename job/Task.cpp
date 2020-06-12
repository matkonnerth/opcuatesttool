#include "Task.h"
#include "Client.h"
#include <chrono>
#include <thread>

namespace tt {

bool ReadRequest::prepare(TestClient* client)
{
   return client->cacheNodeId(id);
}

bool ReadRequest::execute(TestClient* client)
{
   return client->read(id);
}

bool BrowseRequest::prepare(TestClient* client)
{
   return client->cacheNodeId(id);
}

bool BrowseRequest::execute(TestClient* client)
{
   return client->browse(id);
}

bool Wait::prepare(TestClient*)
{
   return true;
}

bool Wait::execute(TestClient*)
{
   std::this_thread::sleep_for(std::chrono::milliseconds(delay));
   return true;
}
} // namespace tt
