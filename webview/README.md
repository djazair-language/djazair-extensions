# Djazair WebView Extension Manual

*(Read this in [Arabic / العربية](README_AR.md))*

The **WebView** extension allows you to build modern desktop applications (Desktop GUI Apps) using web technologies (HTML/CSS/JavaScript) for the frontend user interface, and the Djazair programming language for the backend business logic. Powered by Microsoft Edge **WebView2 Runtime** (using Edge/Chromium) on Windows, it guarantees fast launch times, GPU rendering, and compatibility with modern web standards.

---

## 📋 Table of Contents
1. [Prerequisites & Installation](#1-prerequisites--installation)
2. [App Class & Event Loop](#2-app-class--event-loop)
3. [Window Management & Events](#3-window-management--events)
4. [IPC Bridge & Communication](#4-ipc-bridge--communication)
5. [System Menus & Taskbar Tray](#5-system-menus--taskbar-tray)
6. [Native System Dialogs](#6-native-system-dialogs)
7. [Complete Code Example](#7-complete-code-example)

---

## 1. Prerequisites & Installation

### Operating System Requirements:
*   Windows 10 or Windows 11 (fully updated).
*   **Microsoft Edge WebView2 Runtime** installed on your system (built-in in modern Windows 11 builds).

### Installation via DPM:
```bash
dpm install webview
```

### Building the Native Module:
Because the module contains C++ source files, it must be compiled locally to output `webview.dll`:
```bash
dpm build webview
```
*Requires a C++ compiler supporting at least C++14 standards (e.g., `g++` from MinGW-w64).*

---

## 2. App Class & Event Loop

Instantiate the main application object using `webview.createWindow(options)`. This allows for creating **Multiple Windows** safely.

**Initialization Options (`options` Map):**
*   `"title"` (String): The window title. (Default: `"Djazair App"`).
*   `"width"`, `"height"` (Numbers): Initial window dimensions in pixels. (Default: `1024x768`).
*   `"x"`, `"y"` (Numbers): Initial window position (use `-1` for OS default position).
*   `"resizable"` (Bool): Allow the user to manually resize the window. (Default: `True`).
*   `"frameless"` (Bool): Remove window borders and title bars (useful for fully custom window UI).
*   `"minWidth"`, `"minHeight"` (Numbers): Minimum size constraints.
*   `"closable"` (Bool): Toggle whether the close button is active.
*   `"debug"` (Bool): Enable DevTools and route `console.log()` to the Djazair terminal. (Default: `False`).

### App Control Methods (`App` methods):
*   `run()`: Starts the GUI window event loop. This blocks execution. You only need to call this **once**, even if you have multiple windows.
*   `quit()`: Destroys the window handle and closes the application cleanly.
*   `close()`: Closes the active window.
*   `isValid()`: Returns `True` if the window is open and valid.
*   `onReady(callback)`: Registers a callback triggered before the event loop starts.
*   `onQuit(callback)`: Registers a callback triggered when exiting the application.
*   `onError(callback)`: Catches bridge/internal errors programmatically.
    ```djazair
    app.onError(fn(err)
        print("Webview Error: " + err)
    end)
    ```
*   `setDebug(enable)`: Enable or disable developer features at runtime.

---

## 3. Window Management & Events

Access and configure the active window using `app.window`.

### Visual and Spatial Controls:
*   `setTitle(text)` / `title()`: Set or retrieve the window title (Full Unicode Support).
*   `setSize(width, height)` / `getSize()`: Set or retrieve the window size `[width, height]`.
*   `setPosition(x, y)` / `getPosition()`: Set or retrieve the window position coordinates `[x, y]`.
*   `show()` / `hide()`: Show or hide the window.
*   `minimize()` / `maximize()` / `restore()`: Minimize, maximize, or restore the window.
*   `isMaximized()` / `isMinimized()` / `isVisible()`: Check the current state of the window.
*   `setBackgroundColor(r, g, b, a = 255)`: Sets the background color behind the web view (removes white flashes during page loads).
*   `setResizable(bool)`: Change resizing capabilities dynamically.
*   `setMinimumSize(w, h)` / `setMaximumSize(w, h)`: Set window dimensions boundaries.

### Content Navigation:
*   `navigate(url)`: Load a website URL. If loading a local file path relative to your script (e.g. `"index.html"`), the system maps it securely to `http://djazair.local/index.html` to avoid CORS blocks.
*   `setHtml(htmlString)`: Load a raw HTML string directly into the browser viewport.
*   `eval(jsCode)`: Execute JavaScript code asynchronously in the browser.
*   `initJs(jsCode)`: Preload and execute JavaScript code before any document scripts run.
*   `openDevTools()`: Open the developer inspection window.
*   `reload()` / `goBack()` / `goForward()`: Navigation history and reload controls.
*   `canGoBack()` / `canGoForward()`: Checks history availability.
*   `setZoomLevel(level)` / `zoomLevel()`: Set or retrieve the zoom level.
*   `setContextMenuEnabled(bool)`: Toggle the default browser right-click menu.

### Handling Window Events:
Listen to window events by passing callback functions:
*   `onClose(cb)`: Called when the user attempts to close the window.
*   `onMove(cb)`: Called when the window moves: `fn(x, y)`.
*   `onResize(cb)`: Called when window is resized: `fn(width, height)`.
*   `onFocus(cb)` / `onBlur(cb)`: Called when window gains/loses focus.
*   `onMaximize(cb)` / `onMinimize(cb)` / `onRestore(cb)`: Called on state transitions.
*   `onLoad(cb)`: Called when document has loaded completely.
*   `onNavigate(url)`: Called when loading starts on a new URL.
*   `onTitleChange(title)`: Called when the document changes title.

---

## 4. IPC Bridge & Communication

The `app.bridge` object is the communication gateway between the backend (Djazair) and the frontend web view environment.

### Web View JavaScript API
Available in the global namespace as `window.djazair`:
*   `djazair.invoke(channel, data)`: Invokes a handler registered in Djazair and passes data payload. Returns a JS `Promise` resolving to the data returned by the backend.
*   `djazair.on(channel, callback)`: Listen to events emitted from the Djazair backend.
*   `djazair.send(channel, data)`: Send messages to backend channels.

### Djazair Backend API
*   `app.bridge.on(channel, handler)`: Register a listener for frontend requests.
    ```djazair
    app.bridge.on("saveData", fn(payload)
        print("Data from frontend: " + str(payload))
        return "Saved!" # Value sent back to the JS Promise
    end)
    ```
*   `app.bridge.send(channel, data)`: Emit events to frontend listeners.
    ```djazair
    app.bridge.send("notifyProgress", {"percentage": 80})
    ```
*   `app.bridge.expose(name, handler)`: Expose a Djazair function directly on the browser's global `window` object.
    ```djazair
    app.bridge.expose("add", fn(args)
        return args[0] + args[1]
    end)
    // In JS: window.add([5, 12]).then(sum => console.log(sum));
    ```

---

## 5. System Menus & Taskbar Tray

### Context Menus (Popup Menus)
Build popup menus triggered on right-clicks or button actions:
*   `webview.menuCreate(label)`: Create a menu handle.
*   `webview.menuAddItem(menu, label, callback)`: Append an option with an action callback.
*   `webview.menuAddSeparator(menu)`: Insert a horizontal divisor.
*   `webview.menuCreateSubmenu(menu, label)`: Create a nested submenu (returns a submenu handle).
*   `webview.menuPopup(windowHandle, menu)`: Trigger the menu at the current cursor coordinates. Pass `app.window._handle` as the first argument.

### System Tray Icons (Stub API)
> Note: The Tray system currently exposes the wrapper API but relies on C++ Stubs. The full Win32 implementation is planned for future updates.
*   `webview.trayCreate(label, iconPath)`
*   `webview.traySetIcon(tray, iconPath)`
*   `webview.traySetMenu(tray, menu)`
*   `webview.traySetTooltip(tray, text)`
*   `webview.trayShowBalloon(tray, title, message, timeout = 5)`
*   `webview.trayDestroy(tray)`

### System Toast Notifications
Generate Windows 10/11 system notifications that appear in the Action Center:
*   `webview.notificationShow(title, message, subtitle = "", sound = False, icon = "", timeout = 5)`

---

## 6. Native System Dialogs

Access native operating system dialogs safely formatted for Windows memory logic:

*   `webview.showMessageBox(options)`: Display a message dialog.
    *   *Options:* `"type"` (info / warning / error), `"title"`, `"message"`, `"detail"`, `"buttons"` (ok / yesno / okcancel), `"defaultId"`.
*   `webview.showOpenFileDialog(options)`: File selector dialog.
    *   *Options:* `"title"`, `"defaultPath"`, `"filters"` (e.g. `[{"name": "Docs", "extensions": ["txt", "json"]}]`), `"multiSelections"` (Bool).
    *   *Returns:* File path string, or array of paths if multi-select, or `Null` if cancelled.
*   `webview.showSaveFileDialog(options)`: Save file target selector. Returns selected save path or `Null`.
*   `webview.showOpenFolderDialog(options)`: Directory selector. Returns selected path or `Null`.
*   `webview.showColorPicker(options)`: OS Color dialog. Returns selected color.

---

## 7. Complete Code Example

```djazair
use webview
use file
use json

# 1. Initialize Window App and Enable Debugging
let app = webview.createWindow({
    "title": "Djazair Desktop Sandbox",
    "width": 900,
    "height": 700,
    "debug": True
})

# Catch internal Webview errors
app.onError(fn(err)
    print("Caught webview error: " + err)
end)

app.onReady(fn()
    print("Application is active and ready.")

    # 2. Setup a Native Context Menu
    let contextMenu = webview.menuCreate("Main Context Menu")
    webview.menuAddItem(contextMenu, "Trigger Native Toast", fn()
        webview.notificationShow("WebView Event", "Triggered from Context Menu!", "Djazair Core", True)
    end)
    webview.menuAddSeparator(contextMenu)
    
    # Submenu configuration
    let windowSubMenu = webview.menuCreateSubmenu(contextMenu, "Manage Window")
    webview.menuAddItem(windowSubMenu, "Minimize", fn() app.window.minimize() end)
    webview.menuAddItem(windowSubMenu, "Maximize", fn() app.window.maximize() end)
    webview.menuAddItem(windowSubMenu, "Restore", fn() app.window.restore() end)
    
    webview.menuAddSeparator(contextMenu)
    webview.menuAddItem(contextMenu, "Quit Application", fn() app.quit() end)

    # 3. Handle Bridge Channels
    # Open Context Menu on JS Right Click
    app.bridge.on("showContextMenu", fn(data)
        webview.menuPopup(app.window._handle, contextMenu)
        return True
    end)

    # Select File Dialog from JS
    app.bridge.on("selectFile", fn(data)
        let filePath = webview.showOpenFileDialog({
            "title": "Select Document",
            "filters": [{"name": "Documents", "extensions": ["txt", "json", "dz"]}]
        })
        if !isNull(filePath)
            return filePath
        end
        return "No file selected"
    end)

    # Save File Dialog from JS
    app.bridge.on("saveData", fn(payload)
        let savePath = webview.showSaveFileDialog({
            "title": "Export Data",
            "filters": [{"name": "JSON Document", "extensions": ["json"]}]
        })
        if !isNull(savePath)
            print("Saving data to: " + savePath)
            file.write(savePath, json.stringify(payload))
            return "File exported to " + savePath
        end
        return "Export cancelled"
    end)

    # 4. Set Frontend Viewport HTML and Styles
    app.window.setHtml(`
        <!DOCTYPE html>
        <html>
        <head>
            <meta charset='UTF-8'>
            <style>
                body {
                    font-family: 'Segoe UI', system-ui, sans-serif;
                    background: radial-gradient(circle, #24243e, #0f0c1b);
                    color: #e2e8f0;
                    margin: 0;
                    display: flex;
                    flex-direction: column;
                    align-items: center;
                    justify-content: center;
                    height: 100vh;
                    user-select: none;
                }
                .app-card {
                    background: rgba(255, 255, 255, 0.05);
                    backdrop-filter: blur(10px);
                    border: 1px solid rgba(255,255,255,0.1);
                    padding: 40px;
                    border-radius: 16px;
                    text-align: center;
                    box-shadow: 0 12px 40px rgba(0,0,0,0.5);
                    max-width: 500px;
                    width: 90%;
                }
                h1 {
                    background: linear-gradient(135deg, #a78bfa, #38bdf8);
                    -webkit-background-clip: text;
                    -webkit-text-fill-color: transparent;
                    font-size: 2.5rem;
                    margin-bottom: 8px;
                }
                button {
                    background: linear-gradient(135deg, #8b5cf6, #3b82f6);
                    color: white;
                    border: none;
                    padding: 12px 24px;
                    font-size: 15px;
                    font-weight: 600;
                    border-radius: 8px;
                    cursor: pointer;
                    margin: 8px;
                    transition: all 0.2s ease;
                }
                button:hover {
                    transform: translateY(-2px);
                    box-shadow: 0 4px 12px rgba(139, 92, 246, 0.4);
                }
                #output {
                    margin-top: 20px;
                    padding: 10px;
                    background: rgba(0,0,0,0.3);
                    border-radius: 6px;
                    font-family: monospace;
                    font-size: 14px;
                    color: #38bdf8;
                    word-break: break-all;
                }
            </style>
        </head>
        <body oncontextmenu='event.preventDefault(); window.djazair.invoke("showContextMenu")'>
            <div class='app-card'>
                <h1>Djazair OS Sandbox</h1>
                <p>Right-click anywhere to trigger the native context menu.</p>
                
                <button onclick='triggerOpenFile()'>Open File</button>
                <button onclick='triggerSaveFile()'>Export Data</button>
                
                <div id='output'>Ready. waiting for input...</div>
            </div>
            
            <script>
                function triggerOpenFile() {
                    document.getElementById('output').innerText = 'Opening dialog...';
                    window.djazair.invoke('selectFile').then(path => {
                        document.getElementById('output').innerText = path;
                    });
                }
                
                function triggerSaveFile() {
                    document.getElementById('output').innerText = 'Saving dialog...';
                    window.djazair.invoke('saveData', {data: 'test'}).then(response => {
                        document.getElementById('output').innerText = response;
                    });
                }
            </script>
        </body>
        </html>
    `)
end)

app.run()
```
