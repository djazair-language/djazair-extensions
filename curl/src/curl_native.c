#include "curl_common.h"
#include <stdio.h>

/* ═══════════════════════════════════════════════════════════════
 * Growable receive buffer
 * ═══════════════════════════════════════════════════════════════ */

typedef struct {
    char   *memory;
    size_t  size;
} MemBuf;

static size_t writeCB(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    MemBuf *mem = (MemBuf *)userp;
    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if (!ptr) return 0;
    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    return realsize;
}

static size_t headerCB(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    MemBuf *mem = (MemBuf *)userp;
    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if (!ptr) return 0;
    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    return realsize;
}

/* ═══════════════════════════════════════════════════════════════
 * Curl Context
 * ═══════════════════════════════════════════════════════════════ */

static bool curlGlobalInitialized = false;

static void ensureCurlInit(void) {
    if (!curlGlobalInitialized) {
        curl_global_init(CURL_GLOBAL_DEFAULT);
        curlGlobalInitialized = true;
    }
}

typedef struct {
    CURL              *curl;
    struct curl_slist *headers;
    char              *postBody;
    char              *responseHeaders;
    char              *errorBuf;
} CurlContext;

static void curl_finalizer(void *ptr) {
    if (ptr) {
        CurlContext *ctx = (CurlContext *)ptr;
        if (ctx->headers)         curl_slist_free_all(ctx->headers);
        if (ctx->curl)            curl_easy_cleanup(ctx->curl);
        if (ctx->postBody)        free(ctx->postBody);
        if (ctx->responseHeaders) free(ctx->responseHeaders);
        if (ctx->errorBuf)        free(ctx->errorBuf);
        free(ctx);
    }
}

/* ═══════════════════════════════════════════════════════════════
 * curl_init — Create new Handle
 * ═══════════════════════════════════════════════════════════════ */

DJAZAIR_FUNC(curlInitNative) {
    djazair_check_args(0, argCount);
    ensureCurlInit();
    CURL *curl = curl_easy_init();
    if (!curl) { djazair_error("CurlError: Failed to initialize libcurl handle."); }

    CurlContext *ctx = calloc(1, sizeof(CurlContext));
    ctx->curl     = curl;
    ctx->errorBuf = malloc(CURL_ERROR_SIZE);
    if (ctx->errorBuf) {
        ctx->errorBuf[0] = '\0';
        curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, ctx->errorBuf);
    }

    return djazair_new_resource_with_finalizer(vm, ctx, "Curl", curl_finalizer);
}

/* ═══════════════════════════════════════════════════════════════
 * curl_setopt — Set option
 * ═══════════════════════════════════════════════════════════════ */

DJAZAIR_FUNC(curlSetOptNative) {
    djazair_check_args(3, argCount);
    if (!djazair_is_resource(args[0])) return djazair_bool(false);
    CurlContext *ctx = (CurlContext *)djazair_get_resource(vm, args[0]);
    if (!ctx || !ctx->curl) return djazair_bool(false);

    int option = (int)djazair_get_num(args, 1);
    CURLcode res = CURLE_OK;

    if (djazair_is_string(args[2])) {
        const char *str_val = djazair_get_str(args, 2);
        if (option == CURLOPT_POSTFIELDS) {
            if (ctx->postBody) { free(ctx->postBody); ctx->postBody = NULL; }
            ctx->postBody = strdup(str_val);
            res = curl_easy_setopt(ctx->curl, CURLOPT_POSTFIELDS, ctx->postBody);
        } else {
            res = curl_easy_setopt(ctx->curl, option, str_val);
        }
    } else if (djazair_is_number(args[2])) {
        res = curl_easy_setopt(ctx->curl, option, (long)djazair_get_num(args, 2));
    } else if (djazair_is_bool(args[2])) {
        res = curl_easy_setopt(ctx->curl, option, djazair_get_bool(args, 2) ? 1L : 0L);
    }
    return djazair_bool(res == CURLE_OK);
}

/* ═══════════════════════════════════════════════════════════════
 * curl_setHeaders — Set headers list
 * ═══════════════════════════════════════════════════════════════ */

DJAZAIR_FUNC(curlSetHeadersNative) {
    djazair_check_args(2, argCount);
    if (!djazair_is_resource(args[0]) || !djazair_is_array(args[1]))
        return djazair_bool(false);
    CurlContext *ctx = (CurlContext *)djazair_get_resource(vm, args[0]);
    if (!ctx || !ctx->curl) return djazair_bool(false);

    if (ctx->headers) { curl_slist_free_all(ctx->headers); ctx->headers = NULL; }

    struct curl_slist *list = NULL;
    int count = djazair_array_len(vm, args[1]);
    for (int i = 0; i < count; i++) {
        djazair_value item = djazair_array_get(vm, args[1], i);
        if (djazair_is_string(item))
            list = curl_slist_append(list, AS_CSTRING(item));
    }
    ctx->headers = list;
    CURLcode res = curl_easy_setopt(ctx->curl, CURLOPT_HTTPHEADER, list);
    return djazair_bool(res == CURLE_OK);
}

/* ═══════════════════════════════════════════════════════════════
 * curl_perform — Execute request (captures body + headers)
 * ═══════════════════════════════════════════════════════════════ */

DJAZAIR_FUNC(curlPerformNative) {
    djazair_check_args(1, argCount);
    if (!djazair_is_resource(args[0])) return djazair_null();
    CurlContext *ctx = (CurlContext *)djazair_get_resource(vm, args[0]);
    if (!ctx || !ctx->curl) return djazair_null();

    MemBuf body = { malloc(1), 0 };
    MemBuf hdrs = { malloc(1), 0 };
    body.memory[0] = '\0';
    hdrs.memory[0] = '\0';

    curl_easy_setopt(ctx->curl, CURLOPT_WRITEFUNCTION,  writeCB);
    curl_easy_setopt(ctx->curl, CURLOPT_WRITEDATA,      (void *)&body);
    curl_easy_setopt(ctx->curl, CURLOPT_HEADERFUNCTION, headerCB);
    curl_easy_setopt(ctx->curl, CURLOPT_HEADERDATA,     (void *)&hdrs);

    if (ctx->errorBuf) ctx->errorBuf[0] = '\0';
    CURLcode res = curl_easy_perform(ctx->curl);

    if (ctx->responseHeaders) { free(ctx->responseHeaders); ctx->responseHeaders = NULL; }
    ctx->responseHeaders = hdrs.memory;

    if (res != CURLE_OK) {
        const char *msg = (ctx->errorBuf && ctx->errorBuf[0])
                          ? ctx->errorBuf
                          : curl_easy_strerror(res);
        fprintf(stderr, "[CurlError] %s\n", msg);
        free(body.memory);
        return djazair_null();
    }

    if (ctx->errorBuf) ctx->errorBuf[0] = '\0';
    djazair_value output = djazair_str_l(vm, body.memory, body.size);
    free(body.memory);
    return output;
}

/* ═══════════════════════════════════════════════════════════════
 * curl_getResponseHeaders — Retrieve response headers as Map
 * ═══════════════════════════════════════════════════════════════ */

DJAZAIR_FUNC(curlGetResponseHeadersNative) {
    djazair_check_args(1, argCount);
    if (!djazair_is_resource(args[0])) return djazair_new_map(vm);
    CurlContext *ctx = (CurlContext *)djazair_get_resource(vm, args[0]);
    if (!ctx || !ctx->responseHeaders) return djazair_new_map(vm);

    djazair_value map = djazair_new_map(vm);
    djazair_push(vm, map);

    char *raw  = ctx->responseHeaders;
    char *line = raw;

    while (*line) {
        char *end = line;
        while (*end && *end != '\r' && *end != '\n') end++;
        size_t len = (size_t)(end - line);
        if (len > 0) {
            if (strncmp(line, "HTTP/", 5) != 0) {
                char *colon = memchr(line, ':', len);
                if (colon) {
                    size_t klen   = (size_t)(colon - line);
                    char  *vstart = colon + 1;
                    while (*vstart == ' ' || *vstart == '\t') vstart++;
                    size_t vlen = (size_t)(end - vstart);
                    while (vlen > 0 && (vstart[vlen-1] == ' ' || vstart[vlen-1] == '\t'))
                        vlen--;
                    if (klen > 0 && vlen > 0) {
                        djazair_value key = djazair_str_l(vm, line, klen);
                        djazair_push(vm, key);
                        djazair_value val = djazair_str_l(vm, vstart, vlen);
                        djazair_push(vm, val);
                        djazair_map_set(vm, map, key, val);
                        djazair_pop(vm); /* val */
                        djazair_pop(vm); /* key */
                    }
                }
            }
        }
        while (*end == '\r' || *end == '\n') end++;
        line = end;
    }

    djazair_pop(vm); /* map */
    return map;
}

/* ═══════════════════════════════════════════════════════════════
 * curl_getError — Last error message
 * ═══════════════════════════════════════════════════════════════ */

DJAZAIR_FUNC(curlGetErrorNative) {
    djazair_check_args(1, argCount);
    if (!djazair_is_resource(args[0])) return djazair_str(vm, "");
    CurlContext *ctx = (CurlContext *)djazair_get_resource(vm, args[0]);
    if (!ctx || !ctx->errorBuf || !ctx->errorBuf[0]) return djazair_str(vm, "");
    return djazair_str(vm, ctx->errorBuf);
}

/* ═══════════════════════════════════════════════════════════════
 * curl_cleanup — Explicit cleanup
 * ═══════════════════════════════════════════════════════════════ */

DJAZAIR_FUNC(curlCleanupNative) {
    djazair_check_args(1, argCount);
    if (djazair_is_resource(args[0])) {
        CurlContext *ctx = (CurlContext *)djazair_get_resource(vm, args[0]);
        if (ctx) {
            if (ctx->headers)         { curl_slist_free_all(ctx->headers);  ctx->headers         = NULL; }
            if (ctx->postBody)        { free(ctx->postBody);                 ctx->postBody         = NULL; }
            if (ctx->responseHeaders) { free(ctx->responseHeaders);         ctx->responseHeaders  = NULL; }
            if (ctx->curl)            { curl_easy_cleanup(ctx->curl);        ctx->curl             = NULL; }
        }
    }
    return djazair_null();
}

/* ═══════════════════════════════════════════════════════════════
 * curl_getInfo — Query request info
 * ═══════════════════════════════════════════════════════════════ */

DJAZAIR_FUNC(curlGetInfoNative) {
    djazair_check_args(2, argCount);
    if (!djazair_is_resource(args[0])) return djazair_null();
    CurlContext *ctx = (CurlContext *)djazair_get_resource(vm, args[0]);
    if (!ctx || !ctx->curl) return djazair_null();

    int info = (int)djazair_get_num(args, 1);
    int type = info & CURLINFO_TYPEMASK;

    if (type == CURLINFO_LONG) {
        long v;
        if (curl_easy_getinfo(ctx->curl, info, &v) == CURLE_OK)
            return djazair_num((double)v);
    } else if (type == CURLINFO_STRING) {
        char *v;
        if (curl_easy_getinfo(ctx->curl, info, &v) == CURLE_OK && v)
            return djazair_str(vm, v);
    } else if (type == CURLINFO_DOUBLE) {
        double v;
        if (curl_easy_getinfo(ctx->curl, info, &v) == CURLE_OK)
            return djazair_num(v);
    }
    return djazair_null();
}
