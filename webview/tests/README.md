# WebView Extension — Test Suite

A comprehensive test suite covering every feature of the `webview` extension.
Each test file opens a standalone interactive window with a built-in result panel.

## Running a Test

```bash
djazair tests/01_bridge.dz
djazair tests/02_window.dz
# ... etc
```

> **Build first:** Make sure `webview.dll` is compiled before running.
> ```bash
> build.bat <path\to\djazair-language>
> ```

---

## Test Files

| File | What It Tests |
|------|--------------|
| [`01_bridge.dz`](01_bridge.dz) | **Bridge & IPC** — Promise invoke, auto JSON, all data types, error handling, push events, `off()` |
| [`02_window.dz`](02_window.dz) | **Window Management** — title, size, position, minimize/maximize/restore, zoom, resizable, bg color |
| [`03_events.dz`](03_events.dz) | **Window Events** — onMove, onResize, onFocus, onBlur, onMaximize, onMinimize, onRestore, onLoad |
| [`04_navigation.dz`](04_navigation.dz) | **Navigation & Content** — getUrl, eval(), initJs, bridge.expose(), zoom, canGoBack/Forward |
| [`05_dialogs.dz`](05_dialogs.dz) | **System Dialogs** — MessageBox (3 types), Open/Save/Folder dialogs, multi-select, color picker |
| [`06_menu_tray_notif.dz`](06_menu_tray_notif.dz) | **Menu, Tray & Notifications** — context menu, submenus, tray lifecycle, balloon, toast |
| [`07_stress.dz`](07_stress.dz) | **Stress & Performance** — 100 rapid calls, concurrent Promise.all, 1000-item arrays, deep nesting, unicode, push flood |

---

## Test Structure

Each test file follows the same pattern:

```
┌──────────────────────────────────────────────┐
│  Djazair Backend (.dz)                       │
│  • Registers bridge.on() handlers            │
│  • Performs window/system operations         │
│  • Pushes events via bridge.send()           │
├──────────────────────────────────────────────┤
│  JavaScript Frontend (setHtml)               │
│  • Buttons trigger await djazair.invoke()    │
│  • assert(condition, message) shows ✔/✘      │
│  • Live result panel in the window           │
└──────────────────────────────────────────────┘
```

### Pass/Fail Indicators

- **✔ PASS** (green) — test assertion passed  
- **✘ FAIL** (red) — assertion failed with details  
- **⏱ perf** (orange) — timing/performance information  
- **⬢ event** (purple) — live event received via push

---

## Coverage Summary

| Feature Area | Tests | Key Scenarios |
|---|---|---|
| **Bridge invoke** | `01` | round-trip, all types, errors, concurrent |
| **Bridge send/on/off** | `01` | push events, listener removal |
| **bridge.expose** | `04` | direct window.* binding |
| **Window geometry** | `02` | set/get size, position, zoom |
| **Window states** | `02` | min/max/restore, visible, resizable |
| **Window events** | `03` | 10 distinct event callbacks |
| **Navigation** | `04` | URL, history, eval, initJs |
| **Dialogs** | `05` | 5 dialog types, cancel handling |
| **Menus** | `06` | items, separators, submenus, click events |
| **Tray** | `06` | create, tooltip, balloon, destroy |
| **Notifications** | `06` | toast with subtitle and sound |
| **Performance** | `07` | 100 rapid, 20 concurrent, 1K arrays, unicode |
