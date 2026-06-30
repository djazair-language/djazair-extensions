#include "djazair_api.h"
#include <string.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#endif

static char *read_pipe(FILE *fp)
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
    return buf;
}

DJAZAIR_FUNC(clipboardPaste)
{
    djazair_check_args(0, argCount);
#ifdef _WIN32
    if (!OpenClipboard(NULL))
        return djazair_str(vm, "");
    HANDLE hData = GetClipboardData(CF_UNICODETEXT);
    if (!hData) { CloseClipboard(); return djazair_str(vm, ""); }
    wchar_t *wtext = (wchar_t *)GlobalLock(hData);
    if (!wtext) { CloseClipboard(); return djazair_str(vm, ""); }
    int len = WideCharToMultiByte(CP_UTF8, 0, wtext, -1, NULL, 0, NULL, NULL);
    char *utf8 = (char *)malloc(len);
    if (!utf8) { GlobalUnlock(hData); CloseClipboard(); return djazair_str(vm, ""); }
    WideCharToMultiByte(CP_UTF8, 0, wtext, -1, utf8, len, NULL, NULL);
    GlobalUnlock(hData);
    CloseClipboard();
    djazair_value result = djazair_str(vm, utf8);
    free(utf8);
    return result;
#elif defined(__APPLE__)
    FILE *fp = popen("pbpaste", "r");
    if (!fp) return djazair_str(vm, "");
    char *text = read_pipe(fp);
    int rc = pclose(fp);
    if (!text || rc != 0) { free(text); return djazair_str(vm, ""); }
    djazair_value result = djazair_str(vm, text);
    free(text);
    return result;
#else
    FILE *fp = popen("xclip -o -selection clipboard 2>/dev/null", "r");
    if (!fp) return djazair_str(vm, "");
    char *text = read_pipe(fp);
    int rc = pclose(fp);
    if (!text || rc != 0) { free(text); return djazair_str(vm, ""); }
    djazair_value result = djazair_str(vm, text);
    free(text);
    return result;
#endif
}

DJAZAIR_FUNC(clipboardCopy)
{
    djazair_check_args(1, argCount);
    djazair_check_str(0);
    const char *text = djazair_get_str(args, 0);
#ifdef _WIN32
    if (!OpenClipboard(NULL)) return djazair_bool(false);
    if (!EmptyClipboard()) { CloseClipboard(); return djazair_bool(false); }
    int wlen = MultiByteToWideChar(CP_UTF8, 0, text, -1, NULL, 0);
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, wlen * sizeof(wchar_t));
    if (!hMem) { CloseClipboard(); return djazair_bool(false); }
    wchar_t *wtext = (wchar_t *)GlobalLock(hMem);
    if (!wtext) { GlobalFree(hMem); CloseClipboard(); return djazair_bool(false); }
    MultiByteToWideChar(CP_UTF8, 0, text, -1, wtext, wlen);
    GlobalUnlock(hMem);
    if (!SetClipboardData(CF_UNICODETEXT, hMem)) { GlobalFree(hMem); CloseClipboard(); return djazair_bool(false); }
    CloseClipboard();
    return djazair_bool(true);
#elif defined(__APPLE__)
    FILE *fp = popen("pbcopy", "w");
    if (!fp) return djazair_bool(false);
    fwrite(text, 1, strlen(text), fp);
    int rc = pclose(fp);
    return djazair_bool(rc == 0);
#else
    FILE *fp = popen("xclip -selection clipboard 2>/dev/null", "w");
    if (!fp) return djazair_bool(false);
    fwrite(text, 1, strlen(text), fp);
    int rc = pclose(fp);
    return djazair_bool(rc == 0);
#endif
}

DJAZAIR_FUNC(clipboardClear)
{
    djazair_check_args(0, argCount);
#ifdef _WIN32
    if (!OpenClipboard(NULL)) return djazair_bool(false);
    BOOL ok = EmptyClipboard();
    CloseClipboard();
    return djazair_bool(ok != FALSE);
#elif defined(__APPLE__)
    FILE *fp = popen("pbcopy", "w");
    if (!fp) return djazair_bool(false);
    int rc = pclose(fp);
    return djazair_bool(rc == 0);
#else
    FILE *fp = popen("xclip -selection clipboard 2>/dev/null", "w");
    if (!fp) return djazair_bool(false);
    int rc = pclose(fp);
    return djazair_bool(rc == 0);
#endif
}

DJAZAIR_FUNC(clipboardHas)
{
    djazair_check_args(0, argCount);
#ifdef _WIN32
    return djazair_bool(IsClipboardFormatAvailable(CF_UNICODETEXT) != FALSE);
#elif defined(__APPLE__)
    FILE *fp = popen("pbpaste | head -c 1 | wc -c", "r");
    if (!fp) return djazair_bool(false);
    char buf[16];
    if (!fgets(buf, sizeof(buf), fp)) { pclose(fp); return djazair_bool(false); }
    pclose(fp);
    int count = atoi(buf);
    return djazair_bool(count > 0);
#else
    FILE *fp = popen("xclip -o -selection clipboard 2>/dev/null | head -c 1 | wc -c", "r");
    if (!fp) return djazair_bool(false);
    char buf[16];
    if (!fgets(buf, sizeof(buf), fp)) { pclose(fp); return djazair_bool(false); }
    pclose(fp);
    int count = atoi(buf);
    return djazair_bool(count > 0);
#endif
}

static NativeMethod clipboard_funcs[] = {
    {"paste", clipboardPaste, 0},
    {"copy",  clipboardCopy,  1},
    {"clear", clipboardClear, 0},
    {"has",   clipboardHas,   0},
    {NULL, NULL, 0}
};

DJAZAIR_EXTENSION(clipboard, clipboard_funcs)
