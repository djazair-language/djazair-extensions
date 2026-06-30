#include "djazair_api.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <zlib.h>

#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#endif

static char *read_pipe(FILE *fp, size_t *out_len)
{
    size_t cap = 4096, len = 0;
    char *buf = (char *)malloc(cap);
    if (!buf) return NULL;
    size_t n;
    while ((n = fread(buf + len, 1, cap - len, fp)) > 0)
    {
        len += n;
        if (cap - len < 256)
        {
            cap *= 2;
            char *tmp = (char *)realloc(buf, cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
        }
    }
    buf[len] = '\0';
    if (out_len) *out_len = len;
    return buf;
}

static char *escape_sq(const char *s)
{
    size_t count = 0;
    for (const char *p = s; *p; p++) if (*p == '\'') count++;
    size_t len = strlen(s) + count + 1;
    char *out = (char *)malloc(len);
    if (!out) return NULL;
    char *q = out;
    for (const char *p = s; *p; p++)
    {
        if (*p == '\'') *q++ = '\'';
        *q++ = *p;
    }
    *q = '\0';
    return out;
}

DJAZAIR_FUNC(zipGzip)
{
    djazair_check_args(1, argCount);
    djazair_check_str(0);
    size_t srcLen;
    const char *src = djazair_get_str_l(args, 0, &srcLen);
    if (srcLen == 0) return djazair_str_l(vm, src, 0);

    z_stream strm;
    memset(&strm, 0, sizeof(strm));
    int ret = deflateInit2(&strm, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
                           MAX_WBITS + 16, 8, Z_DEFAULT_STRATEGY);
    if (ret != Z_OK)
        djazair_error("zip.gzip(): failed to initialize compression stream (zlib error %d)", ret);

    uLong destLen = deflateBound(&strm, srcLen);
    char *dest = (char *)malloc(destLen);
    if (!dest) { deflateEnd(&strm); djazair_error("zip.gzip(): failed to allocate compression buffer"); }

    strm.next_in = (Bytef *)src;
    strm.avail_in = srcLen;
    strm.next_out = (Bytef *)dest;
    strm.avail_out = destLen;

    ret = deflate(&strm, Z_FINISH);
    if (ret != Z_STREAM_END)
    {
        free(dest); deflateEnd(&strm);
        djazair_error("zip.gzip(): deflate returned unexpected status %d", ret);
    }

    uLong compLen = strm.total_out;
    deflateEnd(&strm);
    djazair_value result = djazair_str_l(vm, dest, compLen);
    free(dest);
    return result;
}

DJAZAIR_FUNC(zipGunzip)
{
    djazair_check_args(1, argCount);
    djazair_check_str(0);
    size_t srcLen;
    const char *src = djazair_get_str_l(args, 0, &srcLen);
    if (srcLen == 0) return djazair_str_l(vm, src, 0);

    z_stream strm;
    memset(&strm, 0, sizeof(strm));
    int ret = inflateInit2(&strm, MAX_WBITS + 16);
    if (ret != Z_OK)
        djazair_error("zip.gunzip(): failed to initialize decompression stream (zlib error %d)", ret);

    size_t destCap = srcLen * 4 + 1024;
    char *dest = (char *)malloc(destCap);
    if (!dest) { inflateEnd(&strm); djazair_error("zip.gunzip(): failed to allocate decompression buffer"); }

    strm.next_in = (Bytef *)src;
    strm.avail_in = srcLen;
    strm.next_out = (Bytef *)dest;
    strm.avail_out = destCap;

    while ((ret = inflate(&strm, Z_NO_FLUSH)) == Z_OK)
    {
        size_t used = strm.total_out;
        destCap *= 2;
        char *tmp = (char *)realloc(dest, destCap);
        if (!tmp) { free(dest); inflateEnd(&strm); djazair_error("zip.gunzip(): failed to expand decompression buffer"); }
        dest = tmp;
        strm.next_out = (Bytef *)(dest + used);
        strm.avail_out = destCap - used;
    }

    if (ret != Z_STREAM_END)
    {
        free(dest); inflateEnd(&strm);
        djazair_error("zip.gunzip(): inflate returned unexpected status %d (corrupt or invalid gzip data)", ret);
    }

    uLong decompLen = strm.total_out;
    inflateEnd(&strm);
    djazair_value result = djazair_str_l(vm, dest, decompLen);
    free(dest);
    return result;
}

DJAZAIR_FUNC(zipCreate)
{
    djazair_check_args(2, argCount);
    djazair_check_str(0);
    djazair_check_str(1);

    const char *zipPath = djazair_get_str(args, 0);
    const char *srcDir = djazair_get_str(args, 1);
    char cmd[32768];

#ifdef _WIN32
    char *ez = escape_sq(zipPath);
    char *es = escape_sq(srcDir);
    if (!ez || !es) { free(ez); free(es); djazair_error("zip.create(): failed to allocate memory for path escaping"); }
    snprintf(cmd, sizeof(cmd),
        "powershell -NoProfile -Command \"Compress-Archive -Path '%s\\*' -DestinationPath '%s' -Force\"",
        es, ez);
    free(ez); free(es);
#else
    char *ez = escape_sq(zipPath);
    char *es = escape_sq(srcDir);
    if (!ez || !es) { free(ez); free(es); djazair_error("zip.create(): failed to allocate memory for path escaping"); }
    snprintf(cmd, sizeof(cmd), "cd '%s' && zip -r '%s' .", es, ez);
    free(ez); free(es);
#endif

    int rc = system(cmd);
    return djazair_bool(rc == 0);
}

DJAZAIR_FUNC(zipExtract)
{
    djazair_check_args(2, argCount);
    djazair_check_str(0);
    djazair_check_str(1);

    const char *zipPath = djazair_get_str(args, 0);
    const char *destDir = djazair_get_str(args, 1);
    char cmd[32768];

#ifdef _WIN32
    char *ez = escape_sq(zipPath);
    char *ed = escape_sq(destDir);
    if (!ez || !ed) { free(ez); free(ed); djazair_error("zip.extract(): failed to allocate memory for path escaping"); }
    snprintf(cmd, sizeof(cmd),
        "powershell -NoProfile -Command \"Expand-Archive -Path '%s' -DestinationPath '%s' -Force\"",
        ez, ed);
    free(ez); free(ed);
#else
    char *ez = escape_sq(zipPath);
    char *ed = escape_sq(destDir);
    if (!ez || !ed) { free(ez); free(ed); djazair_error("zip.extract(): failed to allocate memory for path escaping"); }
    snprintf(cmd, sizeof(cmd), "unzip -o '%s' -d '%s'", ez, ed);
    free(ez); free(ed);
#endif

    int rc = system(cmd);
    return djazair_bool(rc == 0);
}

DJAZAIR_FUNC(zipList)
{
    djazair_check_args(1, argCount);
    djazair_check_str(0);

    const char *zipPath = djazair_get_str(args, 0);
    char cmd[32768];

#ifdef _WIN32
    char *ez = escape_sq(zipPath);
    if (!ez) djazair_error("zip.list(): failed to allocate memory for path escaping");
    snprintf(cmd, sizeof(cmd),
        "powershell -NoProfile -Command \"Add-Type -AssemblyName System.IO.Compression.FileSystem; [System.IO.Compression.ZipFile]::OpenRead('%s').Entries.Name 2>$null\"",
        ez);
    free(ez);
#else
    char *ez = escape_sq(zipPath);
    if (!ez) djazair_error("zip.list(): failed to allocate memory for path escaping");
    snprintf(cmd, sizeof(cmd), "unzip -Z -1 '%s'", ez);
    free(ez);
#endif

    FILE *fp = popen(cmd, "r");
    if (!fp) return djazair_null();

    size_t outLen;
    char *output = read_pipe(fp, &outLen);
    pclose(fp);

    if (!output || outLen == 0) { free(output); return djazair_new_array(vm); }

    djazair_value arr = djazair_new_array(vm);
    char *line = output;
    char *next;
    while (line && *line)
    {
        next = strchr(line, '\n');
        if (next) *next = '\0';
        size_t llen = strlen(line);
        while (llen > 0 && (line[llen - 1] == '\r' || line[llen - 1] == '\n')) line[--llen] = '\0';
        if (llen > 0) djazair_array_push(vm, arr, djazair_str(vm, line));
        if (!next) break;
        line = next + 1;
    }
    free(output);
    return arr;
}

static NativeMethod zip_funcs[] = {
    {"gzip",   zipGzip,   1},
    {"gunzip", zipGunzip, 1},
    {"create", zipCreate, 2},
    {"extract",zipExtract,2},
    {"list",   zipList,   1},
    {NULL, NULL, 0}
};

DJAZAIR_EXTENSION(zip, zip_funcs)
