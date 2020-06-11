#include "Task.h"
#include "Client.h"
#include <chrono>
#include <thread>

namespace tt {

bool ReadRequest::prepare(Client* client)
{
   return client->cacheNodeId(id);
}

bool ReadRequest::execute(Client* client)
{
   return client->read(id);
}

bool Wait::prepare(Client*)
{
   return true;
}

bool Wait::execute(Client*)
{
   std::this_thread::sleep_for(std::chrono::milliseconds(delay));
   return true;
}
} // namespace tt
