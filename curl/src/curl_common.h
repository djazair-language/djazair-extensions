#ifndef DJAZAIR_CURL_COMMON_H
#define DJAZAIR_CURL_COMMON_H

#include "djazair_api.h"
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>

// Curl native function declarations
DJAZAIR_FUNC(curlInitNative);
DJAZAIR_FUNC(curlSetOptNative);
DJAZAIR_FUNC(curlSetHeadersNative);
DJAZAIR_FUNC(curlPerformNative);
DJAZAIR_FUNC(curlCleanupNative);
DJAZAIR_FUNC(curlGetInfoNative);
DJAZAIR_FUNC(curlGetResponseHeadersNative);
DJAZAIR_FUNC(curlGetErrorNative);

#endif // DJAZAIR_CURL_COMMON_H
