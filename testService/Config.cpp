#include "Config.h"
#include <fstream>
#include <nlohmann/json.hpp>

namespace opctest {

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Config, majorVersion, minorVersion, gitRepository, maxParallelJobs);

void ConfigPersistence::loadConfig()
{
   using nlohmann::json;
   std::ifstream ifs1{ path};

   auto j = json::parse(ifs1);
   currentConfig = j.get<Config>();
}

void ConfigPersistence::persist() const
{
   using nlohmann::json;
   json j = currentConfig;
   std::ofstream out(path);
   out << j;
   out.close();
}
} // namespace opctest
