# التوثيق الشامل لمكتبة WebView في لغة دزاير (Djazair WebView Extension Manual)

تتيح لك إضافة **WebView** بناء تطبيقات سطح مكتب تفاعلية (Desktop GUI Apps) متكاملة وحديثة بالاعتماد على تقنيات الويب (HTML/CSS/JavaScript) للواجهة الرسومية ولغة دزاير للمنطق البرمجي الخلفي. يعتمد المحرك على تغليف **Microsoft WebView2 Runtime** (باستخدام Edge/Chromium) على أنظمة تشغيل ويندوز، مما يضمن سرعة تشغيل عالية ومطابقة كاملة لأحدث معايير الويب.

---

## 📋 جدول المحتويات
1. [متطلبات التشغيل والتثبيت (Prerequisites & Installation)](#1-متطلبات-التشغيل-والتثبيت-prerequisites--installation)
2. [كلاس التطبيق والدورة الزمنية (App Class & Event Loop)](#2-كلاس-التطبيق-والدورة-الزمنية-app-class--event-loop)
3. [التحكم بالنافذة والخصائص (Window Management & Events)](#3-التحكم-بالنافذة-والخصائص-window-management--events)
4. [الجسر البرمجي والتواصل ثنائي الاتجاه (IPC Bridge & Bindings)](#4-الجسر-البرمجي-والتواصل-ثنائي-الاتجاه-ipc-bridge--bindings)
5. [أدوات النظام المتقدمة (Tray, Menu & Notifications)](#5-أدوات-النظام-المتقدمة-tray-menu--notifications)
6. [نوافذ الحوار والتفاعل الأصلية (Native Dialogs)](#6-نوافذ-الحوار-والتفاعل-الأصلية-native-dialogs)
7. [مشروع تطبيقي متكامل (Advanced Desktop Sandbox)](#7-مشروع-تطبيقي-متكامل-advanced-desktop-sandbox)

---

## 1. متطلبات التشغيل والتثبيت (Prerequisites & Installation)

### متطلبات نظام التشغيل:
*   نظام تشغيل Windows 10 أو 11 (محدّث).
*   وجود حزمة **Microsoft Edge WebView2 Runtime** مثبتة على النظام (تأتي مدمجة تلقائياً في النسخ الحديثة لويندوز 11، أو يمكن تنزيلها مجاناً من موقع مايكروسوفت).

### التثبيت عبر DPM:
```bash
dpm install webview
```

### بناء الموديول الثنائي:
بما أن الموديول مكتوب بلغة C++، يجب تصريفه محلياً لإنتاج ملف `webview.dll`:
```bash
dpm build webview
```
*يتطلب البناء وجود مترجم يدعم معايير C++14 على الأقل (مثل `g++` من حزمة MinGW-w64).*

---

## 2. كلاس التطبيق والدورة الزمنية (App Class & Event Loop)

كلاس التطبيق الرئيسي `webview.createWindow(options)` يُرجع كائناً من كلاس `App`.

**خيارات إنشاء التطبيق (`options` Map):**
*   `"title"` (نص): عنوان النافذة الرئيسي.
*   `"width"`, `"height"` (أرقام): الأبعاد الابتدائية للنافذة.
*   `"x"`, `"y"` (أرقام): موقع النافذة على الشاشة (القيمة `-1` تعني ترك الاختيار الافتراضي لنظام التشغيل).
*   `"resizable"` (منطقي): هل يُسمح للمستخدم بتكبير وتصغير حجم النافذة يدوياً.
*   `"frameless"` (منطقي): إخفاء شريط العنوان وإطار النافذة الخارجي بالكامل (مفيد لبناء واجهات مخصصة بالكامل).
*   `"minWidth"`, `"minHeight"` (أرقام): أدنى حجم مسموح لتقليص النافذة.
*   `"closable"` (منطقي): تحديد ما إذا كان زر الإغلاق نشطاً.
*   `"debug"` (منطقي): عند تفعيلها (`True`) سيتم إظهار أدوات المطور (DevTools) تلقائياً، وتوجيه رسائل الـ `console.log()` من الويب مباشرةً إلى ترمينال دزاير.

### دوال إدارة التطبيق (`App` methods):
*   `run()`: بدء تشغيل نافذة التطبيق والدخول في حلقة معالجة الأحداث الرسومية (Event Loop). يعوق هذا الاستدعاء المعالجة اللاحقة حتى إغلاق النافذة.
*   `quit()`: إغلاق النافذة وتحرير جميع الموارد وإنهاء التطبيق.
*   `close()`: إغلاق النافذة النشطة فقط.
*   `onReady(cb)`: تسجيل دالة تُستدعى قبل بدء حلقة الأحداث.
*   `onQuit(cb)`: تسجيل دالة تُستدعى مباشرةً عند إنهاء التطبيق.
*   `setDebug(enable)`: تفعيل أو إلغاء تفعيل وضع الفحص البرمجي أثناء التشغيل.

---

## 3. التحكم بالنافذة والخصائص (Window Management & Events)

يمكنك التحكم بالنافذة عبر الكائن `app.window`.

### دوال التحكم بالخصائص البصرية والموقع:
*   `setTitle(text)` / `title()`: تعيين وجلب عنوان النافذة.
*   `setSize(width, height)` / `getSize()`: تعيين وجلب حجم النافذة كـ `[w, h]`.
*   `setPosition(x, y)` / `getPosition()`: تحريك النافذة وجلب موقعها كـ `[x, y]`.
*   `show()` / `hide()`: إظهار النافذة أو إخفائها من شريط المهام.
*   `minimize()` / `maximize()` / `restore()`: تصغير النافذة لشريط المهام، تكبيرها لملء الشاشة، أو استعادتها للحجم الطبيعي.
*   `isMaximized()` / `isMinimized()` / `isVisible()`: جلب الحالة الحالية للنافذة (ترجع `True` أو `False`).
*   `setBackgroundColor(r, g, b, a = 255)`: تعيين لون خلفية النافذة الأصلية خلف صفحة الويب (مفيد لمنع الوميض الأبيض أثناء التحميل).
*   `setResizable(bool)`: تعديل إمكانية تغيير الحجم ديناميكياً.
*   `setMinimumSize(w, h)` / `setMaximumSize(w, h)`: تحديد قيود الأبعاد القصوى والدنيا.

### دوال تصفح المحتوى والملاحة:
*   `navigate(url)`: تحميل محتوى صفحة ويب. إذا تم تمرير مسار محلي نسبي مثل `"index.html"`، تقوم المكتبة تلقائياً برسمه كـ `http://djazair.local/index.html` بشكل آمن لمنع مشاكل تقييدات CORS للملفات المحلية.
*   `setHtml(htmlString)`: حقن محتوى كود HTML كامل وعرضه مباشرةً.
*   `eval(jsCode)`: تشغيل كود JavaScript داخل المتصفح بشكل غير متزامن.
*   `initJs(jsCode)`: حقن كود جافا سكريبت يُنفذ في بداية تحميل كل مستند ويب (مفيد لتهيئة بيئة العمل).
*   `openDevTools()`: فتح أدوات المطورين لفحص الكود وتصحيح الأخطاء.
*   `reload()` / `goBack()` / `goForward()`: إعادة تحميل الصفحة، الرجوع، والتقدم في سجل الملاحة.
*   `canGoBack()` / `canGoForward()`: التحقق من إمكانية التنقل في السجل.
*   `setZoomLevel(level)` / `zoomLevel()`: تعيين وجلب مستوى تكبير الصفحة (الافتراضي `0`).
*   `setContextMenuEnabled(bool)`: تمكين أو تعطيل القائمة الافتراضية للمتصفح عند النقر يمين الفأرة.

### معالجة أحداث النافذة (Window Events):
يمكنك الاستماع للأحداث البرمجية للنافذة عبر تمرير دالة استدعاء خلفي:
*   `onClose(cb)`: يُستدعى عندما يحاول المستخدم إغلاق النافذة.
*   `onMove(cb)`: يُستدعى عند تحريك النافذة، يمرر الإحداثيات الجديدة `fn(x, y)`.
*   `onResize(cb)`: يُستدعى عند تغيير الحجم، يمرر الأبعاد الجديدة `fn(width, height)`.
*   `onFocus(cb)` / `onBlur(cb)`: عند اكتساب أو فقدان التركيز.
*   `onMaximize(cb)` / `onMinimize(cb)` / `onRestore(cb)`: عند تكبير، تصغير، أو استعادة النافذة.
*   `onLoad(cb)`: يُستدعى عند اكتمال تحميل كود الصفحة بالكامل.
*   `onNavigate(url)`: يُستدعى عندما تبدأ النافذة في الانتقال لعنوان ويب جديد.
*   `onTitleChange(title)`: يُستدعى عند تغير عنوان الصفحة من داخل المتصفح.

---

## 4. الجسر البرمجي والتواصل ثنائي الاتجاه (IPC Bridge & Bindings)

يعمل الجسر البرمجي `app.bridge` كحلقة وصل لتمرير الرسائل بين دزاير والمتصفح.

### واجهة المتصفح البرمجية (JavaScript API)
تتوفر تلقائياً داخل صفحة الويب تحت الاسم `window.djazair` وتوفر الدوال التالية:
*   `djazair.invoke(channel, data)`: استدعاء حدث مسجل في دزاير وتمرير بيانات له. ترجع وعداً (Promise) يُحل بقيمة الإرجاع المرسلة من دزاير.
*   `djazair.on(channel, callback)`: الاستماع لحدث مرسل من لغة دزاير.
*   `djazair.send(channel, data)`: إرسال بيانات لقناة معالجة داخلية.

### واجهة دزاير البرمجية (Djazair API)
*   `app.bridge.on(channel, handler)`: استقبال استدعاء من الويب.
    ```djazair
    app.bridge.on("saveData", fn(payload)
        # payload هو البيانات القادمة من المتصفح
        print("Saving: " + str(payload))
        return "Success" # تُعاد مباشرة إلى الـ Promise في JS
    end)
    ```
*   `app.bridge.send(channel, data)`: بث حدث وبيانات إلى صفحة الويب.
    ```djazair
    app.bridge.send("statusUpdate", {"progress": 75})
    ```
*   `app.bridge.expose(name, handler)`: تسجيل دالة في دزاير مباشرةً على نطاق الـ window في المتصفح لتسهيل مناداة الأكواد.
    ```djazair
    app.bridge.expose("calculate", fn(args)
        return args[0] * args[1]
    end)
    # في جافا سكريبت: window.calculate(5, 10).then(result => console.log(result));
    ```

---

## 5. أدوات النظام المتقدمة (Tray, Menu & Notifications)

### القوائم المنبثقة المخصصة (Context Menus)
يمكنك بناء قوائم خيارات تظهر عند النقر يمين الفأرة أو في أماكن مخصصة:
*   `webview.menuCreate(label)`: إنشاء مقبض قائمة.
*   `webview.menuAddItem(menu, label, callback)`: إضافة عنصر للقائمة مع دالة استدعاء عند النقر عليه.
*   `webview.menuAddSeparator(menu)`: إضافة خط فاصل.
*   `webview.menuCreateSubmenu(menu, label)`: إنشاء قائمة فرعية داخل القائمة الحالية (تُرجع مقبض قائمة فرعية).
*   `webview.menuPopup(windowHandle, menu)`: إظهار القائمة. يجب تمرير `app.window._handle` كمعامل أول.

### أيقونات شريط المهام (System Tray)
أيقونة مصغرة للتطبيق تعمل بجانب الساعة في ويندوز للتشغيل في الخلفية:
*   `webview.trayCreate(label, iconPath)`: إنشاء الأيقونة وتعيين الصورة والنص التلميحي.
*   `webview.traySetIcon(tray, iconPath)`: تحديث الأيقونة.
*   `webview.traySetTooltip(tray, text)`: تعديل تلميح الماوس.
*   `webview.trayShowBalloon(tray, title, message, timeout = 5)`: إظهار مناداة إشعارية من شريط المهام.
*   `webview.trayDestroy(tray)`: حذف الأيقونة وتحرير مواردها.

### إشعارات النظام (System Toast Notifications)
إشعارات ويندوز 10/11 القياسية التي تظهر في مركز الإجراءات:
*   `webview.notificationShow(title, message, subtitle = "", sound = False, icon = "", timeout = 5)`

---

## 6. نوافذ الحوار والتفاعل الأصلية (Native Dialogs)

تتيح الإضافة إظهار صناديق تفاعل نظام التشغيل الأصلية لحفظ واختيار الملفات:

*   `webview.showMessageBox(options)`: عرض صندوق رسالة.
    *   *خيارات:* `"type"` (info / warning / error)، `"title"`، `"message"`، `"detail"`، `"buttons"` (ok / yesno / okcancel)، `"defaultId"` (معرف الزر المفعّل افتراضياً).
*   `webview.showOpenFileDialog(options)`: نافذة اختيار ملف لفتحه.
    *   *خيارات:* `"title"`، `"defaultPath"`، `"filters"` (مصفوفة فلاتر مثل `[{"name": "Images", "extensions": ["png", "jpg"]}]`)، `"multiSelections"` (منطقي للتحديد المتعدد).
    *   *نوع الإرجاع:* مسار الملف كـ String، أو مصفوفة مسارات عند التحديد المتعدد، أو `Null` عند الإلغاء.
*   `webview.showSaveFileDialog(options)`: نافذة تحديد مسار حفظ ملف. ترجع مسار الملف كـ String أو `Null`.
*   `webview.showOpenFolderDialog(options)`: نافذة اختيار مجلد. ترجع مسار المجلد المختار أو `Null`.
*   `webview.showColorPicker(options)`: أداة تحديد الألوان الأصلية. ترجع قيمة اللون المختار.

---

## 7. مشروع تطبيقي متكامل (Advanced Desktop Sandbox)

يوضح هذا المثال كيفية ربط الجسر والأحداث وتصميم قائمة منبثقة تفاعلية وفتح الملفات:

```djazair
use webview

# 1. إنشاء تطبيق النافذة الرسومية وتفعيل وضع التطوير
let app = webview.createWindow({
    "title": "Djazair Advanced GUI Application",
    "width": 950,
    "height": 700,
    "debug": True
})

app.onReady(fn()
    print("Application is active and ready.")

    # 2. إنشاء قائمة منبثقة أصلية
    let rootMenu = webview.menuCreate("Main Context Menu")
    webview.menuAddItem(rootMenu, "Show Native Notification", fn()
        webview.notificationShow("WebView Event", "You clicked from Context Menu!", "Djazair Core", True)
    end)
    webview.menuAddSeparator(rootMenu)
    
    # قائمة فرعية
    let subMenu = webview.menuCreateSubmenu(rootMenu, "Manage Window")
    webview.menuAddItem(subMenu, "Minimize Window", fn() app.window.minimize() end)
    webview.menuAddItem(subMenu, "Maximize Window", fn() app.window.maximize() end)
    webview.menuAddItem(subMenu, "Restore Window", fn() app.window.restore() end)
    
    webview.menuAddSeparator(rootMenu)
    webview.menuAddItem(rootMenu, "Close Application", fn() app.quit() end)

    # 3. ربط أحداث الجسر مع الويب
    # استقبال طلب إظهار القائمة الأصلية عند النقر يمين الفأرة
    app.bridge.on("showContextMenu", fn(data)
        webview.menuPopup(app.window._handle, rootMenu)
        return True
    end)

    # استقبال طلب اختيار ملف من المتصفح
    app.bridge.on("selectFile", fn(data)
        let path = webview.showOpenFileDialog({
            "title": "Select Text Document",
            "filters": [{"name": "Documents", "extensions": ["txt", "json", "dz"]}]
        })
        if !isNull(path)
            return path
        end
        return "No file selected"
    end)

    # استقبال طلب حفظ ملف
    app.bridge.on("saveData", fn(payload)
        let savePath = webview.showSaveFileDialog({
            "title": "Export Results",
            "filters": [{"name": "JSON Document", "extensions": ["json"]}]
        })
        if !isNull(savePath)
            print("Exporting data to: " + savePath)
            # هنا يمكنك كتابة البيانات للمسار المختار
            return "File exported to " + savePath
        end
        return "Export cancelled"
    end)

    # 4. تعيين كود الويب التفاعلي
    app.window.setHtml("
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
        <body oncontextmenu='event.preventDefault(); window.djazair.invoke(\"showContextMenu\")'>
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
    ")
end)

app.run()
```
