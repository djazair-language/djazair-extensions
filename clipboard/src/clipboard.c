#include "djazair_api.h"

#ifdef _WIN32
#include <windows.h>
#endif

DJAZAIR_FUNC(clipboardGetText)
{
    djazair_check_args(0, argCount);
#ifdef _WIN32
    if (!OpenClipboard(NULL))
        return djazair_str(vm, "");
    HANDLE hData = GetClipboardData(CF_UNICODETEXT);
    if (!hData)
    {
        CloseClipboard();
        return djazair_str(vm, "");
    }
    wchar_t *wtext = (wchar_t *)GlobalLock(hData);
    if (!wtext)
    {
        CloseClipboard();
        return djazair_str(vm, "");
    }
    int len = WideCharToMultiByte(CP_UTF8, 0, wtext, -1, NULL, 0, NULL, NULL);
    char *utf8 = (char *)malloc(len);
    if (!utf8)
    {
        GlobalUnlock(hData);
        CloseClipboard();
        return djazair_str(vm, "");
    }
    WideCharToMultiByte(CP_UTF8, 0, wtext, -1, utf8, len, NULL, NULL);
    GlobalUnlock(hData);
    CloseClipboard();
    djazair_value result = djazair_str(vm, utf8);
    free(utf8);
    return result;
#else
    return djazair_str(vm, "");
#endif
}

DJAZAIR_FUNC(clipboardSetText)
{
    djazair_check_args(1, argCount);
    djazair_check_str(0);
    const char *text = djazair_get_str(args, 0);
#ifdef _WIN32
    if (!OpenClipboard(NULL))
        return djazair_bool(false);
    if (!EmptyClipboard())
    {
        CloseClipboard();
        return djazair_bool(false);
    }
    int wlen = MultiByteToWideChar(CP_UTF8, 0, text, -1, NULL, 0);
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, wlen * sizeof(wchar_t));
    if (!hMem)
    {
        CloseClipboard();
        return djazair_bool(false);
    }
    wchar_t *wtext = (wchar_t *)GlobalLock(hMem);
    if (!wtext)
    {
        GlobalFree(hMem);
        CloseClipboard();
        return djazair_bool(false);
    }
    MultiByteToWideChar(CP_UTF8, 0, text, -1, wtext, wlen);
    GlobalUnlock(hMem);
    if (!SetClipboardData(CF_UNICODETEXT, hMem))
    {
        GlobalFree(hMem);
        CloseClipboard();
        return djazair_bool(false);
    }
    CloseClipboard();
    return djazair_bool(true);
#else
    (void)text;
    return djazair_bool(false);
#endif
}

DJAZAIR_FUNC(clipboardClear)
{
    djazair_check_args(0, argCount);
#ifdef _WIN32
    if (!OpenClipboard(NULL))
        return djazair_bool(false);
    BOOL ok = EmptyClipboard();
    CloseClipboard();
    return djazair_bool(ok != FALSE);
#else
    return djazair_bool(false);
#endif
}

DJAZAIR_FUNC(clipboardHasText)
{
    djazair_check_args(0, argCount);
#ifdef _WIN32
    return djazair_bool(IsClipboardFormatAvailable(CF_UNICODETEXT) != FALSE);
#else
    return djazair_bool(false);
#endif
}

static NativeMethod clipboard_funcs[] = {
    {"getText",  clipboardGetText,  0},
    {"setText",  clipboardSetText,  1},
    {"clear",    clipboardClear,    0},
    {"hasText",  clipboardHasText,  0},
    {NULL, NULL, 0}
};

DJAZAIR_EXTENSION(clipboard, clipboard_funcs)
