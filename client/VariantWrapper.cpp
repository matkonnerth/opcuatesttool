#include "VariantWrapper.h"
#include <open62541/types_generated_handling.h>

namespace opctest::client {
VariantWrapper::VariantWrapper()
{
   variant = UA_Variant_new();
   UA_Variant_init(variant);
}

VariantWrapper::~VariantWrapper()
{
   UA_Variant_delete(variant);
}

bool VariantWrapper::isEqual() const
{
  return false;
}
} // namespace opctest::client
