#include <open62541/types_generated.h>


int main()
{
    UA_Variant var;
    UA_NodeId id=UA_NODEID_STRING(2, "myString");
    UA_Variant_setScalar(&var, &id, &UA_TYPES[UA_TYPES_NODEID]);


    char* jsonVal = (char*)calloc(200, 1);
    char* bufPos = jsonVal;
    char* bufEnd = &jsonVal[200];
    UA_encodeJson(&var, &UA_TYPES[UA_TYPES_VARIANT], (uint8_t**)&bufPos, (const uint8_t**)&bufEnd, nullptr, 0, nullptr, 0, UA_TRUE);
    printf("val: %s\n", jsonVal);

    const char* variantValue = "{ \"Type\": 1, \"Body\": 23 }";
    UA_ByteString bs {strlen(variantValue), (UA_Byte*)variantValue};
    
    UA_Variant decoded;
    UA_decodeJson(&bs, &decoded, &UA_TYPES[UA_TYPES_VARIANT]);
    printf("decoded: %d\n", *((UA_Int32*)decoded.data));
    free(jsonVal);
}