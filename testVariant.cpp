#include <open62541/types_generated.h>


int main()
{
    UA_Variant var;
    UA_BuildInfo info;
    memset(&info, 0, sizeof(UA_BuildInfo));
    UA_Variant_setScalar(&var, &info, &UA_TYPES[UA_TYPES_BUILDINFO]);


    char* jsonVal = (char*)calloc(200, 1);
    char* bufPos = jsonVal;
    char* bufEnd = &jsonVal[200];
    UA_encodeJson(&var, &UA_TYPES[UA_TYPES_VARIANT], (uint8_t**)&bufPos, (const uint8_t**)&bufEnd, nullptr, 0, nullptr, 0, UA_TRUE);
    printf("val: %s", jsonVal);
}