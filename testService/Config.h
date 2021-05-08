#pragma once
#include <filesystem>
#include <string>
#include <unordered_map>

namespace opctest {

class Config
{
public:
   int majorVersion;
   int minorVersion;
   std::string gitRepository;
   int maxParallelJobs;
};


class ConfigPersistence
{
public:
   ConfigPersistence(const std::filesystem::path& configFilePath)
   : path{ configFilePath }
   {
      loadConfig();
   }

   const Config& getConfig() const
   {
      return currentConfig;
   }

   void persist() const;

private:
   void loadConfig();
   std::filesystem::path path{};
   Config currentConfig{};
};
} // namespace opctest