#include "JobFactory.h"
#include <fstream>
#include <nlohmann/json.hpp>



/*
{
    "name": "job1",
    "serverUri": "opc.tcp://localhost:4840",
    "script":
         puts(helloWorld("eval chai"));
         for (var i = 0; i < 100; ++i) {
            connect();
            wait(500);
         }"
}
*/


namespace opctest::testrunner {

std::unique_ptr<Job> JobFactory::createFromFile(const std::string& path, const std::string& scriptDir)
{
   using nlohmann::json;
   std::ifstream ifs1{ path };

   auto j = json::parse(ifs1);

   auto name = j.at("name").get<std::string>();
   auto serverUri = j.at("serverUri").get<std::string>();
   auto scriptName = j.at("script").get<std::string>();

   

   return std::make_unique<Job>(name, serverUri, scriptDir + "/" + scriptName);
}
} // namespace opctest::testrunner
