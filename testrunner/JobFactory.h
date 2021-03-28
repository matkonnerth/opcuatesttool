#pragma once
#include "Job.h"
#include <map>
#include <memory>
#include <string>

namespace opctest::testrunner {
class JobFactory
{
public:
   std::unique_ptr<Job> createFromFile(const std::string& path, const std::string& scriptPath);
};
} // namespace opctest::testrunner