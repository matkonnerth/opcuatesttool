#pragma once
#include <string>

namespace tt {

class ReadValueResult;
class Expectation
{
public:
   virtual ~Expectation() = default;
   virtual bool check(const ReadValueResult& result) const = 0;
};

class ValueExpectation : public Expectation
{
public:
   ValueExpectation(const std::string& jsonValueString)
   : value{ jsonValueString }
   {}
   bool check(const ReadValueResult& result) const override;
   const std::string value;
};
} // namespace tt