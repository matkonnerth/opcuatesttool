#include "Result.h"
#include "Expectation.h"

namespace tt {


bool ReadValueResult::check(const Expectation& expec) const
{
   expec.check(*this);
}

} // namespace tt