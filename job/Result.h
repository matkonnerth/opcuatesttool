#pragma once
#include "VariantWrapper.h"
#include <memory>

namespace tt {


class Expectation;
class Result
{
public:
   virtual bool check(const Expectation& exp) const = 0;
   virtual ~Result() = default;
   bool ok{ false };
};

class ReadValueResult : public Result
{
public:
   ReadValueResult()
   : var{ std::make_unique<VariantWrapper>() }
   {}
   ReadValueResult(ReadValueResult&& other)
   {
      ok=other.ok;
      var.swap(other.var);
      other.var.release();
      other.ok=false;
   }
   bool check(const Expectation& exp) const override;
   std::unique_ptr<VariantWrapper> var{nullptr};
};

} // namespace tt