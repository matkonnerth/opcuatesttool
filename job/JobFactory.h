#pragma once
#include "Job.h"
#include <map>
#include <memory>
#include <string>
namespace tt {
class JobFactory
{
public:
   std::unique_ptr<Job> createFromFile(const std::string& path);
};
} // namespace tt