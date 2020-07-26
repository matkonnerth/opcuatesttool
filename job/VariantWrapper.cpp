#include <VariantWrapper.h>
#include <open62541/types_generated_handling.h>

namespace tt {
VariantWrapper::VariantWrapper()
{
   variant = UA_Variant_new();
   UA_Variant_init(variant);
}

VariantWrapper::~VariantWrapper()
{
   UA_Variant_delete(variant);
}

bool VariantWrapper::isEqual(const std::string& jsonVariant) const
{
   UA_Variant decoded;
   UA_ByteString bs = { jsonVariant.length(), (UA_Byte*)jsonVariant.c_str() };
   auto status = UA_decodeJson(&bs, &decoded, &UA_TYPES[UA_TYPES_VARIANT]);
   if (UA_STATUSCODE_GOOD != status)
   {
      return false;
   }
   if (variant->type != decoded.type)
   {
      return false;
   }
   if (variant->arrayLength != decoded.arrayLength)
   {
      return false;
   }
   size_t memsize = variant->type->memSize;
   if (variant->arrayLength > 0)
   {
      memsize = memsize * variant->arrayLength;
   }
   // TODO: if the variant contains pointers, we cannot use memcmp for comparing the data
   if (memcmp(variant->data, decoded.data, memsize) != 0)
   {
      return false;
   }
   return true;
}
} // namespace tt
