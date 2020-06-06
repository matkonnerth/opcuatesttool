#pragma once
#include <string>
#include <memory>
#include "Job.h"

namespace tt
{
class JobFactory
{
public:
    std::unique_ptr<Job> createFromFile(const std::string&path);
};
}