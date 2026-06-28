/*
 * webview_native.cc — Webview Desktop Framework native module
 */
#include <string.h>
#include <stdlib.h>
#include <string>
#include <mutex>
#include <unordered_map>
#include <vector>

extern "C" {
#include "djazair_api.h"
}

#if defined(__APPLE__)
    #include <TargetConditionals.h>
    #include <objc/objc.h>
    #include <objc/message.h>
    #include <objc/runtime.h>
    #include <CoreFoundation/CoreFoundation.h>
#elif defined(__linux__)
    #include <gtk/gtk.h>
#elif defined(_WIN32)
    #include <windows.h>
    #include <shlobj.h>
    #include <shellapi.h>
    #include <commdlg.h>
#endif

#define WEBVIEW_IMPLEMENTATION
#include "webview.h"

// -----------------------------------------------------
// Per-window context
// -----------------------------------------------------
struct WindowContext {
    webview::webview* wv;
    djazairVM*        vm;
    Value             dispatcher;
    Value             close_callback;
    Value             error_callback;
    Value             move_callback;
    Value             resize_callback;
    Value             focus_callback;
    Value             blur_callback;
    Value             maximize_callback;
    Value             minimize_callback;
    Value             restore_callback;
    Value             navigate_callback;
    Value             title_callback;
    Value             load_callback;
    int               id;
    std::string       current_url;
    double            zoom_level;
    bool              has_navigated;
    bool              can_go_forward;

#if defined(WEBVIEW_PLATFORM_WINDOWS)
    WNDPROC original_wndproc;
    HWND    tray_hwnd;
    HICON   tray_icon;
    HICON   last_icon_small;
    HICON   last_icon_big;
    HMENU   hmenu;
    bool    menu_active;
    NOTIFYICONDATAA nid;
    bool    tray_active;
#endif

    WindowContext()
        : wv(nullptr), vm(nullptr)
        , dispatcher(NULL_VAL), close_callback(NULL_VAL), error_callback(NULL_VAL)
        , move_callback(NULL_VAL), resize_callback(NULL_VAL)
        , focus_callback(NULL_VAL), blur_callback(NULL_VAL)
        , maximize_callback(NULL_VAL), minimize_callback(NULL_VAL), restore_callback(NULL_VAL)
        , navigate_callback(NULL_VAL), title_callback(NULL_VAL), load_callback(NULL_VAL)
        , id(0), zoom_level(1.0), has_navigated(false), can_go_forward(false)
#if defined(WEBVIEW_PLATFORM_WINDOWS)
        , original_wndproc(nullptr), tray_hwnd(nullptr), tray_icon(nullptr)
        , last_icon_small(nullptr), last_icon_big(nullptr)
        , hmenu(nullptr), menu_active(false), tray_active(false)
#endif
    {}
};

// -----------------------------------------------------
// Global state: window registry (protected by mutex)
// -----------------------------------------------------
static std::mutex g_ctx_mtx;
static std::unordered_map<int, WindowContext*> g_contexts;
static int g_active_id = 0;
static int g_next_id = 100;
static djazairVM* g_last_vm = nullptr;

// Menu item callback tracking
static std::mutex g_menu_mtx;
static std::unordered_map<int, Value> g_menu_callbacks;
static int g_next_menu_id = 1000;
// -----------------------------------------------------
// GC protection helpers
// -----------------------------------------------------
static void gc_protect(djazairVM* vm, const char* key, int key_len, Value val) {
    ObjString* k = copyString(vm, key, key_len);
    push(vm, OBJ_VAL(k));
    tableSet(vm, &vm->builtins, OBJ_VAL(k), val);
    pop(vm);
}

static void gc_unprotect(djazairVM* vm, const char* key, int key_len) {
    ObjString* k = copyString(vm, key, key_len);
    push(vm, OBJ_VAL(k));
    tableDelete(vm, &vm->builtins, OBJ_VAL(k));
    pop(vm);
}

static std::string gc_key(const char* prefix, int id) {
    return std::string(prefix) + "_" + std::to_string(id);
}

// -----------------------------------------------------
// Close callback invocation
// -----------------------------------------------------
static void invoke_close_callback(WindowContext* c) {
    if (c && c->vm && !IS_NULL(c->close_callback)) {
        Value *savedStackTop = c->vm->stackTop;
        int targetFrame = c->vm->frameCount;
        int savedHandlerCount = c->vm->handlerCount;
        push(c->vm, c->close_callback);
        if (callValue(c->vm, c->close_callback, 0)) {
            run(c->vm, targetFrame);
        }
        c->vm->handlerCount = savedHandlerCount;
        c->vm->stackTop = savedStackTop;
    }
}

static void invoke_callback_0(WindowContext* c, Value cb) {
    if (c && c->vm && !IS_NULL(cb)) {
        Value *savedStackTop = c->vm->stackTop;
        int targetFrame = c->vm->frameCount;
        int savedHandlerCount = c->vm->handlerCount;
        push(c->vm, cb);
        if (callValue(c->vm, cb, 0)) {
            run(c->vm, targetFrame);
        }
        c->vm->handlerCount = savedHandlerCount;
        c->vm->stackTop = savedStackTop;
    }
}

static void invoke_callback_2(WindowContext* c, Value cb, double a, double b) {
    if (c && c->vm && !IS_NULL(cb)) {
        Value *savedStackTop = c->vm->stackTop;
        int targetFrame = c->vm->frameCount;
        int savedHandlerCount = c->vm->handlerCount;
        push(c->vm, cb);
        push(c->vm, djazair_num(a));
        push(c->vm, djazair_num(b));
        if (callValue(c->vm, cb, 2)) {
            run(c->vm, targetFrame);
        }
        c->vm->handlerCount = savedHandlerCount;
        c->vm->stackTop = savedStackTop;
    }
}

// -----------------------------------------------------
// Window proc hooking for close handling (Windows)
// -----------------------------------------------------
#if defined(WEBVIEW_PLATFORM_WINDOWS)
static void menu_handle_command(WindowContext* c, int item_id);
static LRESULT CALLBACK WebviewWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    WindowContext* c = nullptr;
    for (auto& pair : g_contexts) {
        if (pair.second->wv && (HWND)webview_get_window((webview_t)pair.second->wv) == hwnd) {
            c = pair.second;
            break;
        }
    }
    if (!c) return DefWindowProc(hwnd, msg, wParam, lParam);

    switch (msg) {
        case WM_CLOSE:
            if (!IS_NULL(c->close_callback)) {
                c->wv->dispatch([c]() {
                    invoke_close_callback(c);
                    if (c->wv) c->wv->terminate();
                });
                return 0;
            }
            break;

        case WM_MOVE:
            c->wv->dispatch([c, lParam]() {
                invoke_callback_2(c, c->move_callback, (double)(short)LOWORD(lParam), (double)(short)HIWORD(lParam));
            });
            break;

        case WM_SIZE:
            c->wv->dispatch([c, lParam]() {
                invoke_callback_2(c, c->resize_callback, (double)LOWORD(lParam), (double)HIWORD(lParam));
            });
            break;

        case WM_SETFOCUS:
            c->wv->dispatch([c]() { invoke_callback_0(c, c->focus_callback); });
            break;

        case WM_KILLFOCUS:
            c->wv->dispatch([c]() { invoke_callback_0(c, c->blur_callback); });
            break;

        case WM_COMMAND:
            if (lParam == 0) {  // Menu command
                menu_handle_command(c, (int)LOWORD(wParam));
            }
            break;

        case WM_SYSCOMMAND:
            switch (wParam & 0xFFF0) {
                case SC_MAXIMIZE:
                    c->wv->dispatch([c]() { invoke_callback_0(c, c->maximize_callback); });
                    break;
                case SC_MINIMIZE:
                    c->wv->dispatch([c]() { invoke_callback_0(c, c->minimize_callback); });
                    break;
                case SC_RESTORE:
                    c->wv->dispatch([c]() { invoke_callback_0(c, c->restore_callback); });
                    break;
            }
            break;
    }

    if (c->original_wndproc) {
        return CallWindowProc(c->original_wndproc, hwnd, msg, wParam, lParam);
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

static void hook_window_close(WindowContext* c) {
    if (!c || !c->wv) return;
    HWND hwnd = (HWND)webview_get_window((webview_t)c->wv);
    if (!hwnd) return;
    c->original_wndproc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)WebviewWndProc);
}
#else
static void hook_window_close(WindowContext*) {}
#endif

// -----------------------------------------------------
// Helper: extract HWND from webview (Windows only)
// -----------------------------------------------------
#if defined(WEBVIEW_PLATFORM_WINDOWS)
static HWND get_hwnd(webview::webview* wv) {
    return (HWND)webview_get_window((webview_t)wv);
}

static void pump_windows_messages() {
    MSG msg;
    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}
#endif

// -----------------------------------------------------
// JSON helpers for IPC bridge
// -----------------------------------------------------
static std::string json_escape_string(const std::string& raw) {
    std::string escaped;
    escaped.reserve(raw.size() + 2);
    for (unsigned char c : raw) {
        if (c == '"') escaped += "\\\"";
        else if (c == '\\') escaped += "\\\\";
        else if (c == '\n') escaped += "\\n";
        else if (c == '\r') escaped += "\\r";
        else if (c == '\t') escaped += "\\t";
        else escaped += c;
    }
    return "\"" + escaped + "\"";
}

static std::string value_to_json_result(djazairVM *vm, Value value) {
    if (IS_NULL(value)) return "null";
    if (IS_BOOL(value)) return AS_BOOL(value) ? "true" : "false";
    if (IS_NUMBER(value)) {
        char buf[64];
        snprintf(buf, sizeof(buf), "%.17g", AS_NUMBER(value));
        return std::string(buf);
    }
    if (IS_STRING(value)) {
        std::string raw = AS_CSTRING(value);
        bool is_json = !raw.empty() && (
            raw.front() == '{' || raw.front() == '[' || raw.front() == '"' ||
            raw == "null" || raw == "true" || raw == "false" ||
            (raw.front() >= '0' && raw.front() <= '9') || raw.front() == '-'
        );
        return is_json ? raw : json_escape_string(raw);
    }
    char *text = valueToCString(vm, value);
    std::string result = text ? json_escape_string(text) : "null";
    free(text);
    return result;
}

// -----------------------------------------------------
// Macro: get window context from first arg (handle/id)
// -----------------------------------------------------
#define GET_WINDOW(idx) \
    int g_win_id = (int)AS_NUMBER(args[idx]); \
    auto g_win_it = g_contexts.find(g_win_id); \
    if (g_win_it == g_contexts.end()) return djazair_null(); \
    WindowContext* wc = g_win_it->second; \
    if (!wc->wv) return djazair_null()

// ====================================================================
// APP LIFECYCLE
// ====================================================================

extern "C" DJAZAIR_FUNC(nativeAppRun) {
    djazair_check_args(0, argCount);
    WindowContext* active_ctx = nullptr;
    {
        std::lock_guard<std::mutex> lock(g_ctx_mtx);
        auto it = g_contexts.find(g_active_id);
        if (it != g_contexts.end()) active_ctx = it->second;
    }
    if (active_ctx && active_ctx->wv) {
        active_ctx->wv->run();
    }
    return djazair_null();
}

extern "C" DJAZAIR_FUNC(nativeAppQuit) {
    djazair_check_args(0, argCount);
    std::vector<webview::webview*> all_wv;
    {
        std::lock_guard<std::mutex> lock(g_ctx_mtx);
        for (auto &pair : g_contexts) {
            if (pair.second->wv) all_wv.push_back(pair.second->wv);
        }
    }
    for (auto* wv : all_wv) {
        if (wv) wv->terminate();
    }
    return djazair_null();
}

// ====================================================================
// WINDOW CREATION / DESTRUCTION
// ====================================================================

extern "C" DJAZAIR_FUNC(nativeWindowCreate) {
    djazair_check_args(11, argCount);
    djazair_check_str(0); djazair_check_num(1); djazair_check_num(2);
    djazair_check_num(3); djazair_check_num(4);
    djazair_check_bool(5); djazair_check_bool(6);
    djazair_check_num(7); djazair_check_num(8);
    djazair_check_num(9); djazair_check_num(10);

    const char *title = AS_CSTRING(args[0]);
    int width = (int)AS_NUMBER(args[1]);
    int height = (int)AS_NUMBER(args[2]);
    bool frameless = AS_BOOL(args[5]);
    bool resizable = AS_BOOL(args[6]);
    int min_w = (int)AS_NUMBER(args[7]);
    int min_h = (int)AS_NUMBER(args[8]);
    int max_w = (int)AS_NUMBER(args[9]);
    int max_h = (int)AS_NUMBER(args[10]);

    g_last_vm = vm;
    auto c = new WindowContext();
    c->id = g_next_id++;
    c->vm = vm;

    try {
        c->wv = new webview::webview(true, nullptr);
    } catch (...) {
        delete c;
        return djazair_null();
    }

#if defined(WEBVIEW_PLATFORM_WINDOWS)
    // Drain pending WebView2 init messages
    pump_windows_messages();
    // Show the window before any window ops to avoid deadlocks
    HWND hwnd_cr = get_hwnd(c->wv);
    if (hwnd_cr) {
        SetWindowPos(hwnd_cr, NULL, 0, 0, width > 0 ? width : 640,
                     height > 0 ? height : 480,
                     SWP_NOMOVE | SWP_NOZORDER | SWP_SHOWWINDOW);
        UpdateWindow(hwnd_cr);
        pump_windows_messages();
    }
#endif

    c->wv->set_title(title);
    c->wv->set_size(width, height, WEBVIEW_HINT_NONE);
    if (!resizable) c->wv->set_size(width, height, WEBVIEW_HINT_FIXED);
    if (frameless) {
#if defined(WEBVIEW_PLATFORM_WINDOWS)
        HWND hwnd = get_hwnd(c->wv);
        if (hwnd) {
            LONG style = GetWindowLong(hwnd, GWL_STYLE);
            style &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
            SetWindowLong(hwnd, GWL_STYLE, style);
            SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
            pump_windows_messages();
        }
#elif defined(WEBVIEW_PLATFORM_LINUX)
        GtkWidget* gtk_win = GTK_WIDGET(webview_get_window((webview_t)c->wv));
        if (!gtk_widget_get_realized(gtk_win)) gtk_widget_realize(gtk_win);
        gtk_window_set_decorated(GTK_WINDOW(gtk_win), FALSE);
#endif
    }

    if (min_w > 0 && min_h > 0) c->wv->set_size(min_w, min_h, WEBVIEW_HINT_MIN);
    if (max_w > 0 && max_h > 0) c->wv->set_size(max_w, max_h, WEBVIEW_HINT_MAX);

    {
        std::lock_guard<std::mutex> lock(g_ctx_mtx);
        g_contexts[c->id] = c;
        g_active_id = c->id;
    }
    hook_window_close(c);

    return djazair_num((double)c->id);
}

static void menu_cleanup(WindowContext* c) {
    if (!c || !c->vm) return;
    std::lock_guard<std::mutex> lock(g_menu_mtx);
    (void)lock;
    for (auto it = g_menu_callbacks.begin(); it != g_menu_callbacks.end(); ) {
        if (!IS_NULL(it->second)) {
            std::string k = gc_key("__wv_mcb", it->first);
            gc_unprotect(c->vm, k.c_str(), (int)k.length());
        }
        it = g_menu_callbacks.erase(it);
    }
}

extern "C" DJAZAIR_FUNC(nativeWindowDestroy) {
    djazair_check_args(1, argCount);
    int id = (int)AS_NUMBER(args[0]);
    WindowContext* c = nullptr;
    {
        std::lock_guard<std::mutex> lock(g_ctx_mtx);
        auto it = g_contexts.find(id);
        if (it == g_contexts.end()) return djazair_null();
        c = it->second;
    }
#if defined(WEBVIEW_PLATFORM_WINDOWS)
    pump_windows_messages();
#endif
    if (c->wv) { delete c->wv; c->wv = nullptr; }
#if defined(WEBVIEW_PLATFORM_WINDOWS)
    if (c->tray_active) { Shell_NotifyIconA(NIM_DELETE, &c->nid); c->tray_active = false; }
    if (c->tray_hwnd) { DestroyWindow(c->tray_hwnd); c->tray_hwnd = nullptr; }
    if (c->tray_icon) { DestroyIcon(c->tray_icon); c->tray_icon = nullptr; }
    if (c->last_icon_small) { DestroyIcon(c->last_icon_small); c->last_icon_small = nullptr; }
    if (c->last_icon_big) { DestroyIcon(c->last_icon_big); c->last_icon_big = nullptr; }
#endif
    auto gc_cleanup = [&](const char* prefix, Value val) {
        if (!IS_NULL(val)) {
            std::string k = gc_key(prefix, c->id);
            gc_unprotect(c->vm, k.c_str(), (int)k.length());
        }
    };
    gc_cleanup("__wv_disp", c->dispatcher);
    gc_cleanup("__wv_close", c->close_callback);
    gc_cleanup("__wv_move", c->move_callback);
    gc_cleanup("__wv_resize", c->resize_callback);
    gc_cleanup("__wv_focus", c->focus_callback);
    gc_cleanup("__wv_blur", c->blur_callback);
    gc_cleanup("__wv_max", c->maximize_callback);
    gc_cleanup("__wv_min", c->minimize_callback);
    gc_cleanup("__wv_restore", c->restore_callback);
    gc_cleanup("__wv_nav", c->navigate_callback);
    gc_cleanup("__wv_title", c->title_callback);
    gc_cleanup("__wv_load", c->load_callback);
    menu_cleanup(c);
    delete c;
    {
        std::lock_guard<std::mutex> lock(g_ctx_mtx);
        g_contexts.erase(id);
        if (g_active_id == id) g_active_id = 0;
    }
    return djazair_null();
}

// ====================================================================
// WINDOW OPERATIONS
// ====================================================================

extern "C" DJAZAIR_FUNC(nativeWindowSetVirtualHostMapping) {
    djazair_check_args(3, argCount);
    djazair_check_num(0);
    djazair_check_str(1);
    djazair_check_str(2);
    GET_WINDOW(0);
#if defined(WEBVIEW_PLATFORM_WINDOWS)
    ICoreWebView2Controller* controller = (ICoreWebView2Controller*)webview_get_native_handle((webview_t)wc->wv, WEBVIEW_NATIVE_HANDLE_KIND_BROWSER_CONTROLLER);
    if (controller) {
        ICoreWebView2* webview = nullptr;
        HRESULT hr = controller->get_CoreWebView2(&webview);
        if (SUCCEEDED(hr) && webview) {
            static const IID local_IID_ICoreWebView2_3 = {0xA0D6DF20,0x3B92,0x416D,{0xAA,0x0C,0x43,0x7A,0x9C,0x72,0x78,0x57}};
            ICoreWebView2_3* webview3 = nullptr;
            hr = webview->QueryInterface(local_IID_ICoreWebView2_3, (void**)&webview3);
            if (SUCCEEDED(hr) && webview3) {
                const char* host = AS_CSTRING(args[1]);
                const char* folder = AS_CSTRING(args[2]);

                int host_len = MultiByteToWideChar(CP_UTF8, 0, host, -1, NULL, 0);
                wchar_t* w_host = new wchar_t[host_len];
                MultiByteToWideChar(CP_UTF8, 0, host, -1, w_host, host_len);

                int folder_len = MultiByteToWideChar(CP_UTF8, 0, folder, -1, NULL, 0);
                wchar_t* w_folder = new wchar_t[folder_len];
                MultiByteToWideChar(CP_UTF8, 0, folder, -1, w_folder, folder_len);

                HRESULT map_hr = webview3->SetVirtualHostNameToFolderMapping(w_host, w_folder, COREWEBVIEW2_HOST_RESOURCE_ACCESS_KIND_ALLOW);

                delete[] w_host;
                delete[] w_folder;
                webview3->Release();
                webview->Release();

                if (SUCCEEDED(map_hr)) {
                    return djazair_bool(true);
                }
            } else {
                webview->Release();
            }
        }
    }
#endif
    return djazair_bool(false);
}

extern "C" DJAZAIR_FUNC(nativeWindowSetTitle) {
    djazair_check_args(2, argCount);
    GET_WINDOW(0);
#if defined(WEBVIEW_PLATFORM_WINDOWS)
    pump_windows_messages();
#endif
    if (djazair_is_string(args[1])) wc->wv->set_title(AS_CSTRING(args[1]));
    return djazair_null();
}

extern "C" DJAZAIR_FUNC(nativeWindowGetTitle) {
    djazair_check_args(1, argCount);
    GET_WINDOW(0);
#if defined(WEBVIEW_PLATFORM_WINDOWS)
    HWND hwnd = get_hwnd(wc->wv);
    int len = GetWindowTextLengthW(hwnd);
    if (len == 0) return djazair_str(vm, "");
    std::wstring wbuf(len + 1, L'\0');
    GetWindowTextW(hwnd, &wbuf[0], len + 1);
    int utf8_len = WideCharToMultiByte(CP_UTF8, 0, wbuf.c_str(), len, nullptr, 0, nullptr, nullptr);
    std::string utf8_buf(utf8_len, '\0');
    WideCharToMultiByte(CP_UTF8, 0, wbuf.c_str(), len, &utf8_buf[0], utf8_len, nullptr, nullptr);
    return djazair_str(vm, utf8_buf.c_str());
#elif defined(WEBVIEW_PLATFORM_LINUX)
    const gchar *title = gtk_window_get_title(GTK_WINDOW(webview_get_window((webview_t)wc->wv)));
    return djazair_str(vm, title ? title : "");
#else
    return djazair_str(vm, "");
#endif
}

extern "C" DJAZAIR_FUNC(nativeWindowSetSize) {
    djazair_check_args(3, argCount);
    GET_WINDOW(0);
#if defined(WEBVIEW_PLATFORM_WINDOWS)
    pump_windows_messages();
#endif
    if (djazair_is_number(args[1]) && djazair_is_number(args[2]))
        wc->wv->set_size((int)AS_NUMBER(args[1]), (int)AS_NUMBER(args[2]), WEBVIEW_HINT_NONE);
    return djazair_null();
}

extern "C" DJAZAIR_FUNC(nativeWindowGetSize) {
    djazair_check_args(1, argCount);
    GET_WINDOW(0);
    int w = 0, h = 0;
#if defined(WEBVIEW_PLATFORM_WINDOWS)
    HWND hwnd = get_hwnd(wc->wv);
    RECT r;
    GetClientRect(hwnd, &r);
    w = (int)(r.right - r.left); h = (int)(r.bottom - r.top);
#elif defined(WEBVIEW_PLATFORM_LINUX)
    gtk_window_get_size(GTK_WINDOW(webview_get_window((webview_t)wc->wv)), &w, &h);
#endif
    Value arr = djazair_new_array(vm);
    djazair_array_push(vm, arr, djazair_num((double)w));
    djazair_array_push(vm, arr, djazair_num((double)h));
    return arr;
}

extern "C" DJAZAIR_FUNC(nativeWindowSetPosition) {
    djazair_check_args(3, argCount);
    GET_WINDOW(0);
    if (djazair_is_number(args[1]) && djazair_is_number(args[2])) {
#if defined(WEBVIEW_PLATFORM_WINDOWS)
        pump_windows_messages();
        HWND hwnd = get_hwnd(wc->wv);
        SetWindowPos(hwnd, NULL, (int)AS_NUMBER(args[1]), (int)AS_NUMBER(args[2]), 0, 0, SWP_NOSIZE | SWP_NOZORDER);
#elif defined(WEBVIEW_PLATFORM_LINUX)
        gtk_window_move(GTK_WINDOW(webview_get_window((webview_t)wc->wv)), (int)AS_NUMBER(args[1]), (int)AS_NUMBER(args[2]));
#endif
    }
    return djazair_null();
}

extern "C" DJAZAIR_FUNC(nativeWindowGetPosition) {
    djazair_check_args(1, argCount);
    GET_WINDOW(0);
    int x = 0, y = 0;
#if defined(WEBVIEW_PLATFORM_WINDOWS)
    HWND hwnd = get_hwnd(wc->wv);
    RECT r;
    GetWindowRect(hwnd, &r);
    x = (int)r.left; y = (int)r.top;
#elif defined(WEBVIEW_PLATFORM_LINUX)
    gtk_window_get_position(GTK_WINDOW(webview_get_window((webview_t)wc->wv)), &x, &y);
#endif
    Value arr = djazair_new_array(vm);
    djazair_array_push(vm, arr, djazair_num((double)x));
    djazair_array_push(vm, arr, djazair_num((double)y));
    return arr;
}

extern "C" DJAZAIR_FUNC(nativeWindowMinimize) {
    djazair_check_args(1, argCount);
    GET_WINDOW(0);
#if defined(WEBVIEW_PLATFORM_WINDOWS)
    pump_windows_messages();
    ShowWindow(get_hwnd(wc->wv), SW_MINIMIZE);
#elif defined(WEBVIEW_PLATFORM_DARWIN)
    id win = (id)webview_get_window((webview_t)wc->wv);
    objc_msgSend(win, sel_getUid("miniaturize:"), nil);
#elif defined(WEBVIEW_PLATFORM_LINUX)
    gtk_window_iconify(GTK_WINDOW(webview_get_window((webview_t)wc->wv)));
#endif
    return djazair_null();
}

extern "C" DJAZAIR_FUNC(nativeWindowMaximize) {
    djazair_check_args(1, argCount);
    GET_WINDOW(0);
#if defined(WEBVIEW_PLATFORM_WINDOWS)
    pump_windows_messages();
    ShowWindow(get_hwnd(wc->wv), SW_MAXIMIZE);
    wc->wv->dispatch([wc]() { invoke_callback_0(wc, wc->maximize_callback); });
#elif defined(WEBVIEW_PLATFORM_DARWIN)
    id win = (id)webview_get_window((webview_t)wc->wv);
    objc_msgSend(win, sel_getUid("zoom:"), nil);
#elif defined(WEBVIEW_PLATFORM_LINUX)
    gtk_window_maximize(GTK_WINDOW(webview_get_window((webview_t)wc->wv)));
#endif
    return djazair_null();
}

extern "C" DJAZAIR_FUNC(nativeWindowRestore) {
    djazair_check_args(1, argCount);
    GET_WINDOW(0);
#if defined(WEBVIEW_PLATFORM_WINDOWS)
    pump_windows_messages();
    ShowWindow(get_hwnd(wc->wv), SW_RESTORE);
#elif defined(WEBVIEW_PLATFORM_DARWIN)
    id win = (id)webview_get_window((webview_t)wc->wv);
    objc_msgSend(win, sel_getUid("deminiaturize:"), nil);
#elif defined(WEBVIEW_PLATFORM_LINUX)
    gtk_window_unfullscreen(GTK_WINDOW(webview_get_window((webview_t)wc->wv)));
    gtk_window_unmaximize(GTK_WINDOW(webview_get_window((webview_t)wc->wv)));
    gtk_window_present(GTK_WINDOW(webview_get_window((webview_t)wc->wv)));
#endif
    return djazair_null();
}

extern "C" DJAZAIR_FUNC(nativeWindowHide) {
    djazair_check_args(1, argCount);
    GET_WINDOW(0);
#if defined(WEBVIEW_PLATFORM_WINDOWS)
    pump_windows_messages();
    ShowWindow(get_hwnd(wc->wv), SW_HIDE);
#elif defined(WEBVIEW_PLATFORM_DARWIN)
    id win = (id)webview_get_window((webview_t)wc->wv);
    objc_msgSend(win, sel_getUid("orderOut:"), nil);
#elif defined(WEBVIEW_PLATFORM_LINUX)
    gtk_widget_hide(GTK_WIDGET(webview_get_window((webview_t)wc->wv)));
#endif
    return djazair_null();
}

extern "C" DJAZAIR_FUNC(nativeWindowShow) {
    djazair_check_args(1, argCount);
    GET_WINDOW(0);
#if defined(WEBVIEW_PLATFORM_WINDOWS)
    pump_windows_messages();
    ShowWindow(get_hwnd(wc->wv), SW_SHOW);
#elif defined(WEBVIEW_PLATFORM_DARWIN)
    id win = (id)webview_get_window((webview_t)wc->wv);
    objc_msgSend(win, sel_getUid("makeKeyAndOrderFront:"), nil);
#elif defined(WEBVIEW_PLATFORM_LINUX)
    gtk_widget_show(GTK_WIDGET(webview_get_window((webview_t)wc->wv)));
    gtk_window_present(GTK_WINDOW(webview_get_window((webview_t)wc->wv)));
#endif
    return djazair_null();
}

extern "C" DJAZAIR_FUNC(nativeWindowFocus) {
    djazair_check_args(1, argCount);
    GET_WINDOW(0);
#if defined(WEBVIEW_PLATFORM_WINDOWS)
    pump_windows_messages();
    SetFocus(get_hwnd(wc->wv));
#elif defined(WEBVIEW_PLATFORM_LINUX)
    gtk_window_present(GTK_WINDOW(webview_get_window((webview_t)wc->wv)));
#endif
    return djazair_null();
}

extern "C" DJAZAIR_FUNC(nativeWindowIsMaximized) {
    djazair_check_args(1, argCount);
    GET_WINDOW(0);
#if defined(WEBVIEW_PLATFORM_WINDOWS)
    return djazair_bool(IsZoomed(get_hwnd(wc->wv)));
#elif defined(WEBVIEW_PLATFORM_LINUX)
    GdkWindow *gdk_win = gtk_widget_get_window(GTK_WIDGET(webview_get_window((webview_t)wc->wv)));
    if (gdk_win) {
        GdkWindowState state = gdk_window_get_state(gdk_win);
        return djazair_bool(state & GDK_WINDOW_STATE_MAXIMIZED);
    }
    return djazair_bool(false);
#else
    return djazair_bool(false);
#endif
}

extern "C" DJAZAIR_FUNC(nativeWindowIsMinimized) {
    djazair_check_args(1, argCount);
    GET_WINDOW(0);
#if defined(WEBVIEW_PLATFORM_WINDOWS)
    return djazair_bool(IsIconic(get_hwnd(wc->wv)));
#elif defined(WEBVIEW_PLATFORM_LINUX)
    GdkWindow *gdk_win = gtk_widget_get_window(GTK_WIDGET(webview_get_window((webview_t)wc->wv)));
    if (gdk_win) {
        GdkWindowState state = gdk_window_get_state(gdk_win);
        return djazair_bool(state & GDK_WINDOW_STATE_ICONIFIED);
    }
    return djazair_bool(false);
#else
    return djazair_bool(false);
#endif
}

extern "C" DJAZAIR_FUNC(nativeWindowIsVisible) {
    djazair_check_args(1, argCount);
    GET_WINDOW(0);
#if defined(WEBVIEW_PLATFORM_WINDOWS)
    return djazair_bool(IsWindowVisible(get_hwnd(wc->wv)));
#elif defined(WEBVIEW_PLATFORM_LINUX)
    return djazair_bool(gtk_widget_get_visible(GTK_WIDGET(webview_get_window((webview_t)wc->wv))));
#else
    return djazair_bool(true);
#endif
}

extern "C" DJAZAIR_FUNC(nativeWindowSetResizable) {
    djazair_check_args(2, argCount);
    GET_WINDOW(0);
    if (djazair_is_bool(args[1])) {
#if defined(WEBVIEW_PLATFORM_WINDOWS)
        pump_windows_messages();
        HWND hwnd = get_hwnd(wc->wv);
        LONG style = GetWindowLong(hwnd, GWL_STYLE);
        if (AS_BOOL(args[1])) style |= WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
        else style &= ~(WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
        SetWindowLong(hwnd, GWL_STYLE, style);
        SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
#elif defined(WEBVIEW_PLATFORM_LINUX)
        gtk_window_set_resizable(GTK_WINDOW(webview_get_window((webview_t)wc->wv)), AS_BOOL(args[1]) ? TRUE : FALSE);
#endif
    }
    return djazair_null();
}

extern "C" DJAZAIR_FUNC(nativeWindowSetMinSize) {
    djazair_check_args(3, argCount);
    GET_WINDOW(0);
#if defined(WEBVIEW_PLATFORM_WINDOWS)
    pump_windows_messages();
#endif
    if (djazair_is_number(args[1]) && djazair_is_number(args[2]))
        wc->wv->set_size((int)AS_NUMBER(args[1]), (int)AS_NUMBER(args[2]), WEBVIEW_HINT_MIN);
    return djazair_null();
}

extern "C" DJAZAIR_FUNC(nativeWindowSetMaxSize) {
    djazair_check_args(3, argCount);
    GET_WINDOW(0);
#if defined(WEBVIEW_PLATFORM_WINDOWS)
    pump_windows_messages();
#endif
    if (djazair_is_number(args[1]) && djazair_is_number(args[2]))
        wc->wv->set_size((int)AS_NUMBER(args[1]), (int)AS_NUMBER(args[2]), WEBVIEW_HINT_MAX);
    return djazair_null();
}

extern "C" DJAZAIR_FUNC(nativeWindowSetBackgroundColor) {
    djazair_check_args(5, argCount);
    GET_WINDOW(0);
    return djazair_null();
}

// ====================================================================
// WEBVIEW OPERATIONS
// ====================================================================

extern "C" DJAZAIR_FUNC(nativeWindowNavigate) {
    djazair_check_args(2, argCount);
    GET_WINDOW(0);
    if (djazair_is_string(args[1])) {
        wc->current_url = AS_CSTRING(args[1]);
        wc->has_navigated = true;
        wc->can_go_forward = false;
        wc->wv->navigate(wc->current_url.c_str());
    }
    return djazair_null();
}

extern "C" DJAZAIR_FUNC(nativeWindowSetHtml) {
    djazair_check_args(2, argCount);
    GET_WINDOW(0);
    if (djazair_is_string(args[1])) {
        wc->current_url = "about:blank";
        wc->has_navigated = true;
        wc->wv->set_html(AS_CSTRING(args[1]));
    }
    return djazair_null();
}

extern "C" DJAZAIR_FUNC(nativeWindowEval) {
    djazair_check_args(2, argCount);
    GET_WINDOW(0);
    if (djazair_is_string(args[1])) wc->wv->eval(AS_CSTRING(args[1]));
    return djazair_null();
}

extern "C" DJAZAIR_FUNC(nativeWindowInit) {
    djazair_check_args(2, argCount);
    GET_WINDOW(0);
    if (djazair_is_string(args[1])) wc->wv->init(AS_CSTRING(args[1]));
    return djazair_null();
}

extern "C" DJAZAIR_FUNC(nativeWindowReload) {
    djazair_check_args(1, argCount);
    GET_WINDOW(0);
    wc->wv->eval("location.reload()");
    return djazair_null();
}

extern "C" DJAZAIR_FUNC(nativeWindowGoBack) {
    djazair_check_args(1, argCount);
    GET_WINDOW(0);
    wc->can_go_forward = true;
    wc->wv->eval("history.back()");
    return djazair_null();
}

extern "C" DJAZAIR_FUNC(nativeWindowGoForward) {
    djazair_check_args(1, argCount);
    GET_WINDOW(0);
    wc->can_go_forward = false;
    wc->wv->eval("history.forward()");
    return djazair_null();
}

extern "C" DJAZAIR_FUNC(nativeWindowCanGoBack) {
    djazair_check_args(1, argCount);
    GET_WINDOW(0);
    return djazair_bool(wc->has_navigated);
}

extern "C" DJAZAIR_FUNC(nativeWindowCanGoForward) {
    djazair_check_args(1, argCount);
    GET_WINDOW(0);
    return djazair_bool(wc->can_go_forward);
}

extern "C" DJAZAIR_FUNC(nativeWindowSetZoomLevel) {
    djazair_check_args(2, argCount);
    GET_WINDOW(0);
    if (djazair_is_number(args[1])) {
        wc->zoom_level = AS_NUMBER(args[1]);
        char buf[64];
        snprintf(buf, sizeof(buf), "document.body.style.zoom = '%f'", wc->zoom_level);
        wc->wv->eval(buf);
    }
    return djazair_null();
}

extern "C" DJAZAIR_FUNC(nativeWindowGetZoomLevel) {
    djazair_check_args(1, argCount);
    GET_WINDOW(0);
    return djazair_num(wc->zoom_level);
}

extern "C" DJAZAIR_FUNC(nativeWindowOpenDevTools) {
    djazair_check_args(1, argCount);
    GET_WINDOW(0);
#if defined(WEBVIEW_PLATFORM_WINDOWS)
    pump_windows_messages();
    auto* ctrl = (ICoreWebView2Controller*)webview_get_native_handle(
        (webview_t)wc->wv, WEBVIEW_NATIVE_HANDLE_KIND_BROWSER_CONTROLLER);
    if (ctrl) {
        ICoreWebView2* wv2 = nullptr;
        if (SUCCEEDED(ctrl->get_CoreWebView2(&wv2))) {
            wv2->OpenDevToolsWindow();
            wv2->Release();
        }
    }
#elif defined(WEBVIEW_PLATFORM_LINUX)
    auto* insp = (WebKitWebInspector*)webview_get_native_handle(
        (webview_t)wc->wv, WEBVIEW_NATIVE_HANDLE_KIND_BROWSER_CONTROLLER);
    if (insp) webkit_web_inspector_show(insp);
#elif defined(WEBVIEW_PLATFORM_DARWIN)
    id wk = (id)webview_get_native_handle(
        (webview_t)wc->wv, WEBVIEW_NATIVE_HANDLE_KIND_BROWSER_CONTROLLER);
    if (wk) {
        id prefs = (id)objc_msgSend(wk, sel_getUid("preferences"));
        objc_msgSend(prefs, sel_getUid("setDeveloperExtrasEnabled:"), (BOOL)YES);
        id inspector = (id)objc_msgSend(wk, sel_getUid("inspector"));
        objc_msgSend(inspector, sel_getUid("show:"));
    }
#endif
    return djazair_null();
}

extern "C" DJAZAIR_FUNC(nativeWindowGetUrl) {
    djazair_check_args(1, argCount);
    GET_WINDOW(0);
    return djazair_str(vm, wc->current_url.c_str());
}

extern "C" DJAZAIR_FUNC(nativeWindowSetContextMenu) {
    djazair_check_args(2, argCount);
    GET_WINDOW(0);
    if (djazair_is_bool(args[1])) {
#if defined(WEBVIEW_PLATFORM_WINDOWS)
        pump_windows_messages();
        auto* ctrl = (ICoreWebView2Controller*)webview_get_native_handle(
            (webview_t)wc->wv, WEBVIEW_NATIVE_HANDLE_KIND_BROWSER_CONTROLLER);
        if (ctrl) {
            ICoreWebView2* wv2 = nullptr;
            if (SUCCEEDED(ctrl->get_CoreWebView2(&wv2))) {
                ICoreWebView2Settings* settings = nullptr;
                if (SUCCEEDED(wv2->get_Settings(&settings))) {
                    settings->put_AreDefaultContextMenusEnabled(
                        AS_BOOL(args[1]) ? TRUE : FALSE);
                    settings->Release();
                }
                wv2->Release();
            }
        }
#elif defined(WEBVIEW_PLATFORM_LINUX)
        // WebKitGTK: prevent/allow context menu via JS injection
        if (!AS_BOOL(args[1])) {
            wc->wv->eval(
                "document.addEventListener('contextmenu',function(e){"
                "e.stopImmediatePropagation();e.preventDefault();},true)");
        } else {
            wc->wv->eval(
                "document.addEventListener('contextmenu',function(e){"
                "return true;},true)");
        }
#endif
    }
    return djazair_null();
}

// ====================================================================
// CALLBACK SETTERS
// ====================================================================

extern "C" DJAZAIR_FUNC(nativeWindowSetCloseCallback) {
    djazair_check_args(2, argCount);
    GET_WINDOW(0);
    std::string k = gc_key("__wv_close", wc->id);
    if (!IS_NULL(wc->close_callback)) gc_unprotect(wc->vm, k.c_str(), (int)k.length());
    wc->close_callback = args[1];
    if (!IS_NULL(args[1])) gc_protect(vm, k.c_str(), (int)k.length(), args[1]);
    return djazair_null();
}

static void set_callback(WindowContext* wc, djazairVM* vm, const char* prefix, Value& field, Value cb) {
    std::string k = gc_key(prefix, wc->id);
    if (!IS_NULL(field)) gc_unprotect(wc->vm, k.c_str(), (int)k.length());
    field = cb;
    if (!IS_NULL(cb)) gc_protect(vm, k.c_str(), (int)k.length(), cb);
}

extern "C" DJAZAIR_FUNC(nativeWindowSetDispatcher) {
    djazair_check_args(2, argCount);
    GET_WINDOW(0);
    set_callback(wc, vm, "__wv_disp", wc->dispatcher, args[1]);
    return djazair_null();
}

extern "C" DJAZAIR_FUNC(nativeWindowSetErrorCallback) {
    djazair_check_args(2, argCount);
    GET_WINDOW(0);
    set_callback(wc, vm, "__wv_err", wc->error_callback, args[1]);
    return djazair_null();
}

extern "C" DJAZAIR_FUNC(nativeWindowSetMoveCallback) {
    djazair_check_args(2, argCount);
    GET_WINDOW(0);
    set_callback(wc, vm, "__wv_move", wc->move_callback, args[1]);
    return djazair_null();
}

extern "C" DJAZAIR_FUNC(nativeWindowSetResizeCallback) {
    djazair_check_args(2, argCount);
    GET_WINDOW(0);
    set_callback(wc, vm, "__wv_resize", wc->resize_callback, args[1]);
    return djazair_null();
}

extern "C" DJAZAIR_FUNC(nativeWindowSetFocusCallback) {
    djazair_check_args(2, argCount);
    GET_WINDOW(0);
    set_callback(wc, vm, "__wv_focus", wc->focus_callback, args[1]);
    return djazair_null();
}

extern "C" DJAZAIR_FUNC(nativeWindowSetBlurCallback) {
    djazair_check_args(2, argCount);
    GET_WINDOW(0);
    set_callback(wc, vm, "__wv_blur", wc->blur_callback, args[1]);
    return djazair_null();
}

extern "C" DJAZAIR_FUNC(nativeWindowSetMaximizeCallback) {
    djazair_check_args(2, argCount);
    GET_WINDOW(0);
    set_callback(wc, vm, "__wv_max", wc->maximize_callback, args[1]);
    return djazair_null();
}

extern "C" DJAZAIR_FUNC(nativeWindowSetMinimizeCallback) {
    djazair_check_args(2, argCount);
    GET_WINDOW(0);
    set_callback(wc, vm, "__wv_min", wc->minimize_callback, args[1]);
    return djazair_null();
}

extern "C" DJAZAIR_FUNC(nativeWindowSetRestoreCallback) {
    djazair_check_args(2, argCount);
    GET_WINDOW(0);
    set_callback(wc, vm, "__wv_restore", wc->restore_callback, args[1]);
    return djazair_null();
}

extern "C" DJAZAIR_FUNC(nativeWindowSetNavigateCallback) {
    djazair_check_args(2, argCount);
    GET_WINDOW(0);
    set_callback(wc, vm, "__wv_nav", wc->navigate_callback, args[1]);
    return djazair_null();
}

extern "C" DJAZAIR_FUNC(nativeWindowSetTitleCallback) {
    djazair_check_args(2, argCount);
    GET_WINDOW(0);
    set_callback(wc, vm, "__wv_title", wc->title_callback, args[1]);
    return djazair_null();
}

extern "C" DJAZAIR_FUNC(nativeWindowSetLoadCallback) {
    djazair_check_args(2, argCount);
    GET_WINDOW(0);
    set_callback(wc, vm, "__wv_load", wc->load_callback, args[1]);
    return djazair_null();
}

// ====================================================================
// IPC BIND (JS bridge entry)
// ====================================================================

extern "C" DJAZAIR_FUNC(nativeWindowBind) {
    djazair_check_args(3, argCount);
    GET_WINDOW(0);
    if (!djazair_is_string(args[1])) return djazair_null();
    std::string name(AS_CSTRING(args[1]));
    Value callback = args[2];
    int captured_id = wc->id;

    wc->wv->bind(name, [captured_id, vm, name](const std::string& seq, const std::string& req, void*) {
        auto it = g_contexts.find(captured_id);
        if (it == g_contexts.end() || !it->second->wv) { return; }
        WindowContext* wc2 = it->second;
        if (IS_NULL(wc2->dispatcher)) {
            wc2->wv->resolve(seq, 0, "null");
            return;
        }
        push(vm, wc2->dispatcher);
        ObjString* name_arg = copyString(vm, name.c_str(), (int)name.length());
        push(vm, OBJ_VAL(name_arg));
        ObjString* req_arg = copyString(vm, req.c_str(), (int)req.length());
        push(vm, OBJ_VAL(req_arg));
        Value *savedStackTop = vm->stackTop;
        int targetFrame = vm->frameCount;
        std::string result_str = "null";
        if (callValue(vm, wc2->dispatcher, 2)) {
            DjazairResult res = run(vm, targetFrame);
            if (res == DJAZAIR_OK) {
                Value ret = peek(vm, 0);
                result_str = value_to_json_result(vm, ret);
            }
        }
        vm->stackTop = savedStackTop;
        wc2->wv->resolve(seq, 0, result_str);
    }, nullptr);
    return djazair_null();
}

extern "C" DJAZAIR_FUNC(nativeWindowUnbind) {
    djazair_check_args(2, argCount);
    GET_WINDOW(0);
    if (djazair_is_string(args[1])) wc->wv->unbind(AS_CSTRING(args[1]));
    return djazair_null();
}

// ====================================================================
// DIALOGS
// ====================================================================

extern "C" DJAZAIR_FUNC(nativeDialogMessage) {
    djazair_check_args(7, argCount);
    djazair_check_str(0); djazair_check_str(1); djazair_check_str(2);
    djazair_check_str(3); djazair_check_str(4);
    djazair_check_num(5); djazair_check_num(6);

    const char *type = AS_CSTRING(args[0]);
    const char *title = AS_CSTRING(args[1]);
    const char *message = AS_CSTRING(args[2]);
    const char *detail = AS_CSTRING(args[3]);

#if defined(_WIN32)
    UINT uType = MB_OK;
    if (strcmp(type, "info") == 0) uType |= MB_ICONINFORMATION;
    else if (strcmp(type, "warning") == 0) uType |= MB_ICONWARNING;
    else if (strcmp(type, "error") == 0) uType |= MB_ICONERROR;
    else if (strcmp(type, "question") == 0) uType |= MB_ICONQUESTION;

    const char *buttons = AS_CSTRING(args[4]);
    if (strcmp(buttons, "ok") == 0) uType |= MB_OK;
    else if (strcmp(buttons, "ok-cancel") == 0) uType |= MB_OKCANCEL;
    else if (strcmp(buttons, "yes-no") == 0) uType |= MB_YESNO;
    else if (strcmp(buttons, "yes-no-cancel") == 0) uType |= MB_YESNOCANCEL;
    else if (strcmp(buttons, "retry-cancel") == 0) uType |= MB_RETRYCANCEL;

    std::string fullMsg = message;
    if (detail && strlen(detail) > 0) { fullMsg += "\n"; fullMsg += detail; }

    int result = MessageBoxA(NULL, fullMsg.c_str(), title, uType);
    const char *resultStr = "cancel";
    if (result == IDOK) resultStr = "ok";
    else if (result == IDCANCEL) resultStr = "cancel";
    else if (result == IDYES) resultStr = "yes";
    else if (result == IDNO) resultStr = "no";
    else if (result == IDRETRY) resultStr = "retry";
    return djazair_str(vm, resultStr);
#else
    printf("[%s] %s: %s\n", type, title, message);
    return djazair_str(vm, "ok");
#endif
}

extern "C" DJAZAIR_FUNC(nativeDialogOpenFile) {
    djazair_check_args(5, argCount);
    djazair_check_str(0); djazair_check_str(1);
    djazair_check_arr(2); djazair_check_bool(3); djazair_check_arr(4);

    const char *title = AS_CSTRING(args[0]);
    bool multi = AS_BOOL(args[3]);

#if defined(_WIN32)
    OPENFILENAMEA ofn = {0};
    char fileName[32768] = {0};

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = sizeof(fileName);
    ofn.lpstrTitle = title;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_EXPLORER;
    if (multi) ofn.Flags |= OFN_ALLOWMULTISELECT;

    std::string filterStr;
    Value filters = args[2];
    int filterLen = djazair_array_len(vm, filters);
    for (int i = 0; i < filterLen; i++) {
        Value filter = djazair_array_get(vm, filters, i);
        Value nameKey = djazair_str(vm, "name");
        Value extKey = djazair_str(vm, "extensions");
        Value nameVal = NULL_VAL, extVal = NULL_VAL;
        djazair_map_get(vm, filter, nameKey, &nameVal);
        djazair_map_get(vm, filter, extKey, &extVal);
        const char *fname = IS_STRING(nameVal) ? AS_CSTRING(nameVal) : "Files";
        filterStr += fname; filterStr += '\0';
        if (IS_ARRAY(extVal)) {
            int extLen = djazair_array_len(vm, extVal);
            for (int j = 0; j < extLen; j++) {
                Value ext = djazair_array_get(vm, extVal, j);
                if (IS_STRING(ext)) {
                    if (j > 0) filterStr += ";";
                    std::string extStr = AS_CSTRING(ext);
                    if (extStr.size() >= 2 && extStr.substr(0, 2) == "*.") extStr = extStr.substr(2);
                    filterStr += "*."; filterStr += extStr;
                }
            }
        }
        filterStr += '\0';
    }
    if (filterStr.empty()) filterStr = "All Files\0*.*\0";
    else filterStr += '\0';
    ofn.lpstrFilter = filterStr.data();

    if (GetOpenFileNameA(&ofn)) {
        if (!multi) {
            return djazair_str(vm, fileName);
        }
        // Parse multi-select result: directory\0file1\0file2\0\0
        std::string dir(fileName);
        char* p = fileName + dir.length() + 1;
        if (*p == '\0') {
            // Only one file selected
            return djazair_str(vm, fileName);
        }
        Value arr = djazair_new_array(vm);
        std::string sep = "\\";
        if (dir.back() == '\\') sep = "";
        while (*p) {
            std::string full = dir + sep + p;
            djazair_array_push(vm, arr, djazair_str(vm, full.c_str()));
            p += strlen(p) + 1;
        }
        return arr;
    }
#endif
    return djazair_null();
}

extern "C" DJAZAIR_FUNC(nativeDialogSaveFile) {
    djazair_check_args(3, argCount);
    djazair_check_str(0); djazair_check_str(1); djazair_check_arr(2);

    const char *title = AS_CSTRING(args[0]);
    const char *defaultPath = AS_CSTRING(args[1]);

#if defined(_WIN32)
    OPENFILENAMEA ofn = {0};
    char fileName[MAX_PATH] = {0};
    if (defaultPath && strlen(defaultPath) > 0) strncpy(fileName, defaultPath, MAX_PATH - 1);

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrTitle = title;
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;

    std::string filterStr = "All Files\0*.*\0";
    ofn.lpstrFilter = filterStr.data();

    if (GetSaveFileNameA(&ofn)) return djazair_str(vm, fileName);
#endif
    return djazair_null();
}

extern "C" DJAZAIR_FUNC(nativeDialogOpenFolder) {
    djazair_check_args(2, argCount);
    djazair_check_str(0); djazair_check_str(1);

    const char *title = AS_CSTRING(args[0]);
#if defined(_WIN32)
    BROWSEINFOA bi = {0};
    bi.lpszTitle = title;
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
    LPITEMIDLIST pidl = SHBrowseForFolderA(&bi);
    if (pidl) {
        char path[MAX_PATH];
        if (SHGetPathFromIDListA(pidl, path)) {
            IMalloc *imalloc = NULL;
            if (SUCCEEDED(SHGetMalloc(&imalloc))) { imalloc->Free(pidl); imalloc->Release(); }
            return djazair_str(vm, path);
        }
        IMalloc *imalloc = NULL;
        if (SUCCEEDED(SHGetMalloc(&imalloc))) { imalloc->Free(pidl); imalloc->Release(); }
    }
#endif
    return djazair_null();
}

extern "C" DJAZAIR_FUNC(nativeDialogPickColor) {
    djazair_check_args(1, argCount);
    djazair_check_str(0);

#if defined(_WIN32)
    CHOOSECOLORA cc = {0};
    COLORREF crCust[16] = {0};
    cc.lStructSize = sizeof(cc);
    cc.hwndOwner = NULL;
    cc.lpCustColors = crCust;
    cc.Flags = CC_RGBINIT | CC_FULLOPEN;
    if (ChooseColorA(&cc)) {
        Value result = djazair_new_map(vm);
        djazair_map_set(vm, result, djazair_str(vm, "r"), djazair_num(GetRValue(cc.rgbResult)));
        djazair_map_set(vm, result, djazair_str(vm, "g"), djazair_num(GetGValue(cc.rgbResult)));
        djazair_map_set(vm, result, djazair_str(vm, "b"), djazair_num(GetBValue(cc.rgbResult)));
        djazair_map_set(vm, result, djazair_str(vm, "a"), djazair_num(255));
        return result;
    }
#endif
    return djazair_null();
}

// ====================================================================
// MENU (Windows HMENU implementation)
// ====================================================================



extern "C" DJAZAIR_FUNC(nativeMenuCreate) {
    djazair_check_args(1, argCount);
    djazair_check_str(0);
#if defined(WEBVIEW_PLATFORM_WINDOWS)
    HMENU hmenu = CreatePopupMenu();
    return djazair_num((double)(intptr_t)hmenu);
#else
    return djazair_null();
#endif
}

extern "C" DJAZAIR_FUNC(nativeMenuCreateSubmenu) {
    djazair_check_args(2, argCount);
    djazair_check_str(1);
#if defined(WEBVIEW_PLATFORM_WINDOWS)
    HMENU parent = (HMENU)(intptr_t)AS_NUMBER(args[0]);
    HMENU sub = CreatePopupMenu();
    AppendMenuA(parent, MF_STRING | MF_POPUP, (UINT_PTR)sub, AS_CSTRING(args[1]));
    return djazair_num((double)(intptr_t)sub);
#else
    return djazair_null();
#endif
}

extern "C" DJAZAIR_FUNC(nativeMenuAddSeparator) {
    djazair_check_args(1, argCount);
#if defined(WEBVIEW_PLATFORM_WINDOWS)
    HMENU hmenu = (HMENU)(intptr_t)AS_NUMBER(args[0]);
    AppendMenuA(hmenu, MF_SEPARATOR, 0, NULL);
#endif
    return djazair_null();
}

extern "C" DJAZAIR_FUNC(nativeMenuAddItem) {
    djazair_check_args(3, argCount);
    djazair_check_str(1);
#if defined(WEBVIEW_PLATFORM_WINDOWS)
    HMENU hmenu = (HMENU)(intptr_t)AS_NUMBER(args[0]);
    const char* label = AS_CSTRING(args[1]);
    int item_id = g_next_menu_id++;
    AppendMenuA(hmenu, MF_STRING, item_id, label);
    Value cb = args[2];
    if (!IS_NULL(cb)) {
        std::string k = gc_key("__wv_mcb", item_id);
        gc_protect(vm, k.c_str(), (int)k.length(), cb);
        std::lock_guard<std::mutex> lock(g_menu_mtx);
        (void)lock;
        g_menu_callbacks[item_id] = cb;
    }
#endif
    return djazair_null();
}

extern "C" DJAZAIR_FUNC(nativeMenuSetCallback) {
    djazair_check_args(3, argCount);
    // Stub — callbacks set during menuAddItem now
    return djazair_null();
}

static void menu_handle_command(WindowContext* c, int item_id) {
    Value cb = NULL_VAL;
    {
        std::lock_guard<std::mutex> lock(g_menu_mtx);
        (void)lock;
        auto it = g_menu_callbacks.find(item_id);
        if (it != g_menu_callbacks.end()) cb = it->second;
    }
    if (!IS_NULL(cb) && c && c->wv) {
        c->wv->dispatch([c, cb]() {
            invoke_callback_0(c, cb);
        });
    }
}

extern "C" DJAZAIR_FUNC(nativeMenuPopup) {
    djazair_check_args(2, argCount);
    djazair_check_num(1);
#if defined(WEBVIEW_PLATFORM_WINDOWS)
    int win_id = (int)AS_NUMBER(args[0]);
    HMENU hmenu = (HMENU)(intptr_t)AS_NUMBER(args[1]);
    HWND hwnd = nullptr;
    {
        std::lock_guard<std::mutex> lock(g_ctx_mtx);
        (void)lock;
        auto it = g_contexts.find(win_id);
        if (it != g_contexts.end() && it->second->wv) {
            hwnd = get_hwnd(it->second->wv);
        }
    }
    if (hwnd && hmenu) {
        POINT pt;
        GetCursorPos(&pt);
        TrackPopupMenu(hmenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
    }
#endif
    return djazair_null();
}

// ====================================================================
// NOTIFICATIONS
// ====================================================================

extern "C" DJAZAIR_FUNC(nativeNotificationShow) {
    djazair_check_args(6, argCount);
    djazair_check_str(0); djazair_check_str(1); djazair_check_str(2);
    djazair_check_bool(3); djazair_check_str(4); djazair_check_num(5);

#if defined(_WIN32)
    NOTIFYICONDATAA nid = {0};
    nid.cbSize = sizeof(NOTIFYICONDATAA);
    nid.uFlags = NIF_INFO | NIF_GUID;
    nid.dwInfoFlags = NIIF_INFO;
    nid.uTimeout = (UINT)AS_NUMBER(args[5]);
    strncpy(nid.szInfoTitle, AS_CSTRING(args[0]), sizeof(nid.szInfoTitle) - 1);
    nid.szInfoTitle[sizeof(nid.szInfoTitle) - 1] = '\0';
    strncpy(nid.szInfo, AS_CSTRING(args[1]), sizeof(nid.szInfo) - 1);
    nid.szInfo[sizeof(nid.szInfo) - 1] = '\0';
    Shell_NotifyIconA(NIM_ADD, &nid);
    Shell_NotifyIconA(NIM_DELETE, &nid);
#endif
    return djazair_null();
}

// ====================================================================
// TRAY
// ====================================================================

extern "C" DJAZAIR_FUNC(nativeTrayCreate) {
    djazair_check_args(2, argCount);
    djazair_check_str(0); djazair_check_str(1);
    return djazair_num(1.0);
}

extern "C" DJAZAIR_FUNC(nativeTraySetIcon) {
    djazair_check_args(2, argCount);
    djazair_check_num(0); djazair_check_str(1);
    return djazair_null();
}

extern "C" DJAZAIR_FUNC(nativeTraySetTooltip) {
    djazair_check_args(2, argCount);
    djazair_check_num(0); djazair_check_str(1);
    return djazair_null();
}

extern "C" DJAZAIR_FUNC(nativeTrayDestroy) {
    djazair_check_args(1, argCount);
    djazair_check_num(0);
    return djazair_null();
}

extern "C" DJAZAIR_FUNC(nativeTrayShowBalloon) {
    djazair_check_args(4, argCount);
    djazair_check_num(0); djazair_check_str(1);
    djazair_check_str(2); djazair_check_num(3);
    return djazair_null();
}

// ====================================================================
// PROTOCOL (stubs)
// ====================================================================

extern "C" DJAZAIR_FUNC(nativeProtocolRegister) {
    djazair_check_args(2, argCount);
    djazair_check_str(0);
    return djazair_null();
}

extern "C" DJAZAIR_FUNC(nativeProtocolUnregister) {
    djazair_check_args(1, argCount);
    djazair_check_str(0);
    return djazair_null();
}

// ====================================================================
// MODULE REGISTRATION
// ====================================================================

static NativeMethod webview_methods[] = {
    // App lifecycle
    {"appRun",               nativeAppRun,               0},
    {"appQuit",              nativeAppQuit,              0},

    // Window management
    {"windowCreate",         nativeWindowCreate,         11},
    {"windowDestroy",        nativeWindowDestroy,        1},
    {"windowSetTitle",       nativeWindowSetTitle,       2},
    {"windowGetTitle",       nativeWindowGetTitle,       1},
    {"windowSetSize",        nativeWindowSetSize,        3},
    {"windowGetSize",        nativeWindowGetSize,        1},
    {"windowSetPosition",    nativeWindowSetPosition,    3},
    {"windowGetPosition",    nativeWindowGetPosition,    1},
    {"windowMinimize",       nativeWindowMinimize,       1},
    {"windowMaximize",       nativeWindowMaximize,       1},
    {"windowRestore",        nativeWindowRestore,        1},
    {"windowHide",           nativeWindowHide,           1},
    {"windowShow",           nativeWindowShow,           1},
    {"windowFocus",          nativeWindowFocus,          1},
    {"windowIsMaximized",    nativeWindowIsMaximized,    1},
    {"windowIsMinimized",    nativeWindowIsMinimized,    1},
    {"windowIsVisible",      nativeWindowIsVisible,      1},
    {"windowSetResizable",   nativeWindowSetResizable,   2},
    {"windowSetMinSize",     nativeWindowSetMinSize,     3},
    {"windowSetMaxSize",     nativeWindowSetMaxSize,     3},
    {"windowSetBackgroundColor", nativeWindowSetBackgroundColor, 5},
    {"windowSetVirtualHostMapping", nativeWindowSetVirtualHostMapping, 3},

    // WebView
    {"windowNavigate",       nativeWindowNavigate,       2},
    {"windowSetHtml",        nativeWindowSetHtml,        2},
    {"windowEval",           nativeWindowEval,           2},
    {"windowInit",           nativeWindowInit,           2},
    {"windowReload",         nativeWindowReload,         1},
    {"windowGoBack",         nativeWindowGoBack,         1},
    {"windowGoForward",      nativeWindowGoForward,      1},
    {"windowCanGoBack",      nativeWindowCanGoBack,      1},
    {"windowCanGoForward",   nativeWindowCanGoForward,   1},
    {"windowSetZoomLevel",   nativeWindowSetZoomLevel,   2},
    {"windowGetZoomLevel",   nativeWindowGetZoomLevel,   1},
    {"windowOpenDevTools",   nativeWindowOpenDevTools,   1},
    {"windowGetUrl",         nativeWindowGetUrl,         1},
    {"windowSetContextMenu", nativeWindowSetContextMenu, 2},
    {"windowBind",           nativeWindowBind,           3},
    {"windowUnbind",         nativeWindowUnbind,         2},

    // Callbacks
    {"windowSetCloseCallback",   nativeWindowSetCloseCallback,   2},
    {"windowSetDispatcher",      nativeWindowSetDispatcher,      2},
    {"windowSetErrorCallback",   nativeWindowSetErrorCallback,   2},
    {"windowSetMoveCallback",    nativeWindowSetMoveCallback,    2},
    {"windowSetResizeCallback",  nativeWindowSetResizeCallback,  2},
    {"windowSetFocusCallback",   nativeWindowSetFocusCallback,   2},
    {"windowSetBlurCallback",    nativeWindowSetBlurCallback,    2},
    {"windowSetMaximizeCallback",nativeWindowSetMaximizeCallback,2},
    {"windowSetMinimizeCallback",nativeWindowSetMinimizeCallback,2},
    {"windowSetRestoreCallback", nativeWindowSetRestoreCallback, 2},
    {"windowSetNavigateCallback",nativeWindowSetNavigateCallback,2},
    {"windowSetTitleCallback",   nativeWindowSetTitleCallback,   2},
    {"windowSetLoadCallback",    nativeWindowSetLoadCallback,    2},

    // Dialogs
    {"dialogMessage",        nativeDialogMessage,        7},
    {"dialogOpenFile",       nativeDialogOpenFile,       5},
    {"dialogSaveFile",       nativeDialogSaveFile,       3},
    {"dialogOpenFolder",     nativeDialogOpenFolder,     2},
    {"dialogPickColor",      nativeDialogPickColor,      1},

    // Menu
    {"menuCreate",           nativeMenuCreate,           1},
    {"menuCreateSubmenu",    nativeMenuCreateSubmenu,    2},
    {"menuAddSeparator",     nativeMenuAddSeparator,     1},
    {"menuAddItem",          nativeMenuAddItem,          3},
    {"menuSetCallback",      nativeMenuSetCallback,      3},
    {"menuPopup",            nativeMenuPopup,            2},

    // Notifications
    {"notificationShow",     nativeNotificationShow,     6},

    // Tray
    {"trayCreate",           nativeTrayCreate,           2},
    {"traySetIcon",          nativeTraySetIcon,          2},
    {"traySetTooltip",       nativeTraySetTooltip,       2},
    {"trayDestroy",          nativeTrayDestroy,          1},
    {"trayShowBalloon",      nativeTrayShowBalloon,      4},

    // Protocol
    {"protocolRegister",     nativeProtocolRegister,     2},
    {"protocolUnregister",   nativeProtocolUnregister,   1},

    {NULL, NULL, 0}
};

extern "C" {
    DJAZAIR_EXTENSION(webview, webview_methods)
}
