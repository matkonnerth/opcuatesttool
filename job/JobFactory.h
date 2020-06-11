#pragma once
#include "Job.h"
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <map>
namespace tt {
using CreateJobFnc = std::unique_ptr<Job> (*)(const std::string& name, const std::string& serverUri, const nlohmann::json& j);
class JobFactory
{
public:
   std::unique_ptr<Job> createFromFile(const std::string& path);

private:
   static const std::unordered_map<std::string, CreateJobFnc> jobs;
};
} // namespace tt