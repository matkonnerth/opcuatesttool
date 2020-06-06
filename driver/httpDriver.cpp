#include "pistache/endpoint.h"

using namespace Pistache;
class HelloHandler : public Http::Handler {
public:

    HTTP_PROTOTYPE(HelloHandler)

    void onRequest(const Http::Request& request, Http::ResponseWriter response) {
         response.send(Http::Code::Ok, "Hello, World");
    }
}; 

int main(int argc, const char* argv[])
{
Pistache::Address addr(Pistache::Ipv4::any(), Pistache::Port(9080));
    auto opts = Pistache::Http::Endpoint::options()
        .threads(1);

    Http::Endpoint server(addr);
    server.init(opts);
    server.setHandler(Http::make_handler<HelloHandler>());
    server.serve();
}