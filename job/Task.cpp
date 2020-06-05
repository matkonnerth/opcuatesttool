#include "Task.h"
#include "Client.h"

namespace tt
{

    void ReadRequest::prepare(Client* client)
    {
        client->cacheNodeId(id);
    }

    void ReadRequest::execute(Client* client)
    {
        client->read(id);
    }
}
