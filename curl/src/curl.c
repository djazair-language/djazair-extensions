#include "curl_common.h"
#include <stddef.h>

static NativeMethod curl_module_funcs[] = {
    {"curlInit",                curlInitNative,               0},
    {"curlSetOpt",              curlSetOptNative,             3},
    {"curlSetHeaders",          curlSetHeadersNative,         2},
    {"curlPerform",             curlPerformNative,            1},
    {"curlCleanup",             curlCleanupNative,            1},
    {"curlGetInfo",             curlGetInfoNative,            2},
    {"curlGetResponseHeaders",  curlGetResponseHeadersNative, 1},
    {"curlGetError",            curlGetErrorNative,           1},
    {NULL, NULL, 0}
};

DJAZAIR_EXTENSION(curl, curl_module_funcs)
