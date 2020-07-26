#include "Expectation.h"
#include "Result.h"

namespace tt {
bool ValueExpectation::check(const ReadValueResult& result) const
{
   return result.var->isEqual(value);
}
} // namespace tt