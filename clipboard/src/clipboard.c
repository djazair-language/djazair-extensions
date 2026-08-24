/**
 * [ Djazair Programming Language Extension ]
 * Module: clipboard
 * Developer: Harizi Riyadh (hariziriyadh@gmail.com)
 *
 * clipboard.c — Production-grade native clipboard extension for Djazair
 * =====================================================================
 * Implements high-resilience, cross-platform system clipboard operations:
 * - Windows: Win32 API (user32.dll) with transient lock retry loops,
 *   UTF-8/UTF-16 conversions, sequence number tracking, and format enumeration.
 * - Linux: Automatic detection for Wayland (wl-clipboard) and X11 (xclip / xsel).
 * - macOS: Native pbcopy / pbpaste integration.
 */

#include "djazair_api.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
    #include <sys/types.h>
#endif

/* ============================================================
 * Windows Helper Functions & Resilience
 * ============================================================ */

#ifdef _WIN32

#define CLIPBOARD_MAX_RETRIES 10
#define CLIPBOARD_RETRY_DELAY_MS 5

/**
 * Attempts to open the Windows clipboard with retry logic.
 * Solves transient locking issues caused by clipboard managers, browsers, or office apps.
 */
static BOOL open_clipboard_with_retry(HWND owner)
{
    for (int i = 0; i < CLIPBOARD_MAX_RETRIES; i++)
    {
        if (OpenClipboard(owner))
        {
            return TRUE;
        }
        Sleep(CLIPBOARD_RETRY_DELAY_MS);
    }
    return FALSE;
}

#else

/* ============================================================
 * POSIX / Linux / macOS Helper Functions
 * ============================================================ */

/**
 * Reads all data from a command pipe safely into a dynamic memory buffer.
 */
static char *read_pipe(FILE *fp, size_t *out_len)
{
    size_t capacity = 4096;
    size_t length = 0;
    char *buffer = (char *)malloc(capacity);
    if (!buffer)
    {
        return NULL;
    }

    size_t bytes_read;
    while ((bytes_read = fread(buffer + length, 1, capacity - length - 1, fp)) > 0)
    {
        length += bytes_read;
        if (capacity - length < 512)
        {
            capacity *= 2;
            char *resized = (char *)realloc(buffer, capacity);
            if (!resized)
            {
                free(buffer);
                return NULL;
            }
            buffer = resized;
        }
    }

    buffer[length] = '\0';
    if (out_len)
    {
        *out_len = length;
    }
    return buffer;
}

/**
 * Checks if a CLI command is available in the current PATH.
 */
static bool command_exists(const char *cmd)
{
    char check_cmd[256];
    snprintf(check_cmd, sizeof(check_cmd), "command -v %s >/dev/null 2>&1", cmd);
    return (system(check_cmd) == 0);
}

#endif

/* ============================================================
 * Native Method Implementations
 * ============================================================ */

/**
 * clipboard.paste() -> String
 * Retrieves the current text content from the system clipboard as UTF-8 string.
 */
DJAZAIR_FUNC(clipboardPaste)
{
    djazair_check_args(0, argCount);

#ifdef _WIN32
    if (!open_clipboard_with_retry(NULL))
    {
        return djazair_str(vm, "");
    }

    // Check for Unicode text format first (CF_UNICODETEXT)
    HANDLE hData = GetClipboardData(CF_UNICODETEXT);
    if (!hData)
    {
        CloseClipboard();
        return djazair_str(vm, "");
    }

    const wchar_t *wtext = (const wchar_t *)GlobalLock(hData);
    if (!wtext)
    {
        CloseClipboard();
        return djazair_str(vm, "");
    }

    // Determine required UTF-8 buffer length (including terminating null byte)
    int utf8_len = WideCharToMultiByte(CP_UTF8, 0, wtext, -1, NULL, 0, NULL, NULL);
    if (utf8_len <= 1)
    {
        GlobalUnlock(hData);
        CloseClipboard();
        return djazair_str(vm, "");
    }

    char *utf8 = (char *)malloc((size_t)utf8_len);
    if (!utf8)
    {
        GlobalUnlock(hData);
        CloseClipboard();
        return djazair_str(vm, "");
    }

    WideCharToMultiByte(CP_UTF8, 0, wtext, -1, utf8, utf8_len, NULL, NULL);
    GlobalUnlock(hData);
    CloseClipboard();

    // Create binary-safe Djazair string (length is utf8_len - 1 without trailing null byte)
    djazair_value result = djazair_str_l(vm, utf8, (size_t)(utf8_len - 1));
    free(utf8);
    return result;

#elif defined(__APPLE__)
    FILE *fp = popen("pbpaste", "r");
    if (!fp)
    {
        return djazair_str(vm, "");
    }

    size_t length = 0;
    char *text = read_pipe(fp, &length);
    int rc = pclose(fp);

    if (!text || rc != 0)
    {
        free(text);
        return djazair_str(vm, "");
    }

    djazair_value result = djazair_str_l(vm, text, length);
    free(text);
    return result;

#else
    // Linux: Check for Wayland first, then fall back to X11 (xclip / xsel)
    const char *cmd = NULL;
    if (getenv("WAYLAND_DISPLAY") && command_exists("wl-paste"))
    {
        cmd = "wl-paste -n 2>/dev/null";
    }
    else if (command_exists("xclip"))
    {
        cmd = "xclip -o -selection clipboard 2>/dev/null";
    }
    else if (command_exists("xsel"))
    {
        cmd = "xsel -o -b 2>/dev/null";
    }

    if (!cmd)
    {
        return djazair_str(vm, "");
    }

    FILE *fp = popen(cmd, "r");
    if (!fp)
    {
        return djazair_str(vm, "");
    }

    size_t length = 0;
    char *text = read_pipe(fp, &length);
    int rc = pclose(fp);

    if (!text || rc != 0)
    {
        free(text);
        return djazair_str(vm, "");
    }

    djazair_value result = djazair_str_l(vm, text, length);
    free(text);
    return result;
#endif
}

/**
 * clipboard.copy(text) -> Bool
 * Places a UTF-8 string into the system clipboard.
 */
DJAZAIR_FUNC(clipboardCopy)
{
    djazair_check_args(1, argCount);
    djazair_check_str(0);

    size_t text_len = 0;
    const char *text = djazair_get_str_l(args, 0, &text_len);

#ifdef _WIN32
    if (!open_clipboard_with_retry(NULL))
    {
        return djazair_bool(false);
    }

    if (!EmptyClipboard())
    {
        CloseClipboard();
        return djazair_bool(false);
    }

    // Calculate required wide character buffer length
    int wlen = MultiByteToWideChar(CP_UTF8, 0, text, (int)text_len, NULL, 0);
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, (size_t)(wlen + 1) * sizeof(wchar_t));
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

    MultiByteToWideChar(CP_UTF8, 0, text, (int)text_len, wtext, wlen);
    wtext[wlen] = L'\0';
    GlobalUnlock(hMem);

    // If SetClipboardData succeeds, the system owns the memory handle
    if (!SetClipboardData(CF_UNICODETEXT, hMem))
    {
        GlobalFree(hMem);
        CloseClipboard();
        return djazair_bool(false);
    }

    CloseClipboard();
    return djazair_bool(true);

#elif defined(__APPLE__)
    FILE *fp = popen("pbcopy", "w");
    if (!fp)
    {
        return djazair_bool(false);
    }

    if (text_len > 0)
    {
        fwrite(text, 1, text_len, fp);
    }
    int rc = pclose(fp);
    return djazair_bool(rc == 0);

#else
    // Linux: Support Wayland (wl-copy) or X11 (xclip / xsel)
    const char *cmd = NULL;
    if (getenv("WAYLAND_DISPLAY") && command_exists("wl-copy"))
    {
        cmd = "wl-copy 2>/dev/null";
    }
    else if (command_exists("xclip"))
    {
        cmd = "xclip -selection clipboard 2>/dev/null";
    }
    else if (command_exists("xsel"))
    {
        cmd = "xsel -b -i 2>/dev/null";
    }

    if (!cmd)
    {
        return djazair_bool(false);
    }

    FILE *fp = popen(cmd, "w");
    if (!fp)
    {
        return djazair_bool(false);
    }

    if (text_len > 0)
    {
        fwrite(text, 1, text_len, fp);
    }
    int rc = pclose(fp);
    return djazair_bool(rc == 0);
#endif
}

/**
 * clipboard.clear() -> Bool
 * Empties all contents from the system clipboard.
 */
DJAZAIR_FUNC(clipboardClear)
{
    djazair_check_args(0, argCount);

#ifdef _WIN32
    if (!open_clipboard_with_retry(NULL))
    {
        return djazair_bool(false);
    }

    BOOL ok = EmptyClipboard();
    CloseClipboard();
    return djazair_bool(ok != FALSE);

#elif defined(__APPLE__)
    FILE *fp = popen("pbcopy", "w");
    if (!fp)
    {
        return djazair_bool(false);
    }
    int rc = pclose(fp);
    return djazair_bool(rc == 0);

#else
    // Linux clear
    const char *cmd = NULL;
    if (getenv("WAYLAND_DISPLAY") && command_exists("wl-copy"))
    {
        cmd = "wl-copy -c 2>/dev/null";
    }
    else if (command_exists("xclip"))
    {
        cmd = "xclip -selection clipboard /dev/null 2>/dev/null";
    }
    else if (command_exists("xsel"))
    {
        cmd = "xsel -b -c 2>/dev/null";
    }

    if (!cmd)
    {
        return djazair_bool(false);
    }

    FILE *fp = popen(cmd, "w");
    if (!fp)
    {
        return djazair_bool(false);
    }
    int rc = pclose(fp);
    return djazair_bool(rc == 0);
#endif
}

/**
 * clipboard.has() -> Bool
 * Checks if the clipboard currently contains text data.
 */
DJAZAIR_FUNC(clipboardHas)
{
    djazair_check_args(0, argCount);

#ifdef _WIN32
    // Check both standard Unicode and ANSI text formats
    return djazair_bool(IsClipboardFormatAvailable(CF_UNICODETEXT) != FALSE ||
                        IsClipboardFormatAvailable(CF_TEXT) != FALSE);

#elif defined(__APPLE__)
    FILE *fp = popen("pbpaste | head -c 1 | wc -c", "r");
    if (!fp)
    {
        return djazair_bool(false);
    }
    char buf[32];
    if (!fgets(buf, sizeof(buf), fp))
    {
        pclose(fp);
        return djazair_bool(false);
    }
    pclose(fp);
    return djazair_bool(atoi(buf) > 0);

#else
    // Linux has text check
    const char *cmd = NULL;
    if (getenv("WAYLAND_DISPLAY") && command_exists("wl-paste"))
    {
        cmd = "wl-paste -n 2>/dev/null | head -c 1 | wc -c";
    }
    else if (command_exists("xclip"))
    {
        cmd = "xclip -o -selection clipboard 2>/dev/null | head -c 1 | wc -c";
    }
    else if (command_exists("xsel"))
    {
        cmd = "xsel -o -b 2>/dev/null | head -c 1 | wc -c";
    }

    if (!cmd)
    {
        return djazair_bool(false);
    }

    FILE *fp = popen(cmd, "r");
    if (!fp)
    {
        return djazair_bool(false);
    }
    char buf[32];
    if (!fgets(buf, sizeof(buf), fp))
    {
        pclose(fp);
        return djazair_bool(false);
    }
    pclose(fp);
    return djazair_bool(atoi(buf) > 0);
#endif
}

/**
 * clipboard.sequenceNumber() -> Number
 * Returns a 32-bit sequence number that increments on every clipboard modification.
 * Allows O(1) change detection without reading or copying entire clipboard payloads.
 */
DJAZAIR_FUNC(clipboardSequenceNumber)
{
    djazair_check_args(0, argCount);

#ifdef _WIN32
    DWORD seq = GetClipboardSequenceNumber();
    return djazair_num((double)seq);
#else
    // POSIX fallback: maintain a local monotonic counter
    static unsigned long fallback_seq = 1;
    return djazair_num((double)(fallback_seq++));
#endif
}

/**
 * clipboard.formats() -> Array
 * Enumerates all data formats currently present in the system clipboard,
 * returning an array of human-readable format names (e.g., ["unicode", "text", "bitmap", "files"]).
 */
DJAZAIR_FUNC(clipboardFormats)
{
    djazair_check_args(0, argCount);

    djazair_value arr = djazair_new_array(vm);

#ifdef _WIN32
    if (!open_clipboard_with_retry(NULL))
    {
        return arr;
    }

    UINT format = 0;
    while ((format = EnumClipboardFormats(format)) != 0)
    {
        const char *name = NULL;
        char custom_name[128] = {0};

        switch (format)
        {
            case CF_UNICODETEXT: name = "unicode"; break;
            case CF_TEXT:        name = "text"; break;
            case CF_OEMTEXT:     name = "oem_text"; break;
            case CF_BITMAP:      name = "bitmap"; break;
            case CF_DIB:         name = "dib"; break;
            case CF_DIBV5:       name = "dibv5"; break;
            case CF_HDROP:       name = "files"; break;
            case CF_WAVE:        name = "wave"; break;
            case CF_RIFF:        name = "riff"; break;
            case CF_LOCALE:      name = "locale"; break;
            default:
                if (GetClipboardFormatNameA(format, custom_name, sizeof(custom_name)) > 0)
                {
                    name = custom_name;
                }
                else
                {
                    name = "custom";
                }
                break;
        }

        if (name && strlen(name) > 0)
        {
            djazair_array_push(vm, arr, djazair_str(vm, name));
        }
    }

    CloseClipboard();
    return arr;

#else
    // POSIX fallback: report text if available
    djazair_value has_val = clipboardHas(vm, 0, args);
    if (djazair_is_bool(has_val) && AS_BOOL(has_val))
    {
        djazair_array_push(vm, arr, djazair_str(vm, "text"));
        djazair_array_push(vm, arr, djazair_str(vm, "unicode"));
    }
    return arr;
#endif
}

/* ============================================================
 * Module Method Registration
 * ============================================================ */

static NativeMethod clipboard_funcs[] = {
    {"paste",          clipboardPaste,          0},
    {"copy",           clipboardCopy,           1},
    {"clear",          clipboardClear,          0},
    {"has",            clipboardHas,            0},
    {"sequenceNumber", clipboardSequenceNumber, 0},
    {"formats",        clipboardFormats,        0},
    {NULL,             NULL,                    0}
};

DJAZAIR_EXTENSION(clipboard, clipboard_funcs)
