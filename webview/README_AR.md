# دليل إضافة Djazair WebView

*(Read this in [English](README.md))*

تسمح لك إضافة **WebView** ببناء تطبيقات سطح مكتب حديثة (Desktop GUI Apps) باستخدام تقنيات الويب (HTML/CSS/JavaScript) لواجهة المستخدم الأمامية (Frontend)، ولغة برمجة **Djazair** لمعالجة المنطق الخلفي (Backend).
تعتمد الإضافة على محرك **Microsoft Edge WebView2** (مبني على تقنية Edge/Chromium) في بيئة Windows، مما يضمن سرعة في الإقلاع، تسريعاً عبر معالج الرسوميات (GPU)، وتوافقية تامة مع أحدث معايير الويب.

---

## 📋 جدول المحتويات
1. [المتطلبات والتثبيت](#1-المتطلبات-والتثبيت)
2. [كلاس التطبيق وحلقة الأحداث (Event Loop)](#2-كلاس-التطبيق-وحلقة-الأحداث)
3. [إدارة النوافذ والأحداث](#3-إدارة-النوافذ-والأحداث)
4. [جسر الاتصال (IPC Bridge)](#4-جسر-الاتصال-ipc-bridge)
5. [قوائم النظام وشريط المهام (Tray)](#5-قوائم-النظام-وشريط-المهام)
6. [نوافذ الحوار النظامية (Dialogs)](#6-نوافذ-الحوار-النظامية)
7. [مثال برمجي متكامل](#7-مثال-برمجي-متكامل)

---

## 1. المتطلبات والتثبيت

### متطلبات نظام التشغيل:
*   نظام Windows 10 أو Windows 11 (مُحدَّث بالكامل).
*   **Microsoft Edge WebView2 Runtime** مُثبت على جهازك (مُدمج بشكل افتراضي في إصدارات Windows 11 الحديثة).

### التثبيت عبر مدير الحزم (DPM):
```bash
dpm install webview
```

### بناء الوحدة الأصلية (Native Module):
نظراً لاحتواء الإضافة على ملفات مصدرية بلغة C++، يجب تجميعها محلياً لإنتاج ملف `webview.dll`:
```bash
dpm build webview
```
*يتطلب هذا وجود مترجم C++ يدعم معايير C++14 كحد أدنى (مثل `g++` من بيئة MinGW-w64).*

---

## 2. كلاس التطبيق وحلقة الأحداث

يتم إنشاء كائن التطبيق الرئيسي باستخدام الدالة `webview.createWindow(options)`. تتيح هذه الدالة إمكانية إنشاء **نوافذ متعددة (Multiple Windows)** بأمان.

**خيارات التهيئة (خريطة `options`):**
*   `"title"` (نص): عنوان النافذة. (الافتراضي: `"Djazair App"`).
*   `"width"`, `"height"` (أرقام): الأبعاد الأولية للنافذة بالبكسل. (الافتراضي: `1024x768`).
*   `"x"`, `"y"` (أرقام): إحداثيات موضع النافذة الأولي (استخدم `-1` لترك التحديد لنظام التشغيل).
*   `"resizable"` (منطقي): السماح للمستخدم بتغيير حجم النافذة. (الافتراضي: `True`).
*   `"frameless"` (منطقي): إزالة حواف النافذة وشريط العنوان (مفيد لتصميم واجهات مخصصة بالكامل).
*   `"minWidth"`, `"minHeight"` (أرقام): قيود الحجم الأدنى للنافذة.
*   `"closable"` (منطقي): تفعيل أو تعطيل زر الإغلاق.
*   `"debug"` (منطقي): تفعيل أدوات المطورين (DevTools) وتوجيه رسائل `console.log()` من الـ JS إلى نافذة الـ Terminal. (الافتراضي: `False`).

### دوال التحكم في التطبيق (ميثودات `App`):
*   `run()`: تبدأ حلقة أحداث النافذة (Event Loop). يؤدي هذا لتعليق تنفيذ الكود الذي يليه. **تحتاج لاستدعاء هذه الدالة مرة واحدة فقط** حتى لو كان لديك عدة نوافذ.
*   `quit()`: تدمير مقبض النافذة وإغلاق التطبيق بشكل نظيف.
*   `close()`: إغلاق النافذة النشطة.
*   `isValid()`: تُرجع `True` إذا كانت النافذة مفتوحة وصالحة للاستخدام.
*   `onReady(callback)`: تسجيل دالة تُستدعى قبل بدء حلقة الأحداث.
*   `onQuit(callback)`: تسجيل دالة تُستدعى عند خروج التطبيق.
*   `onError(callback)`: التقاط أخطاء الـ Bridge أو الأخطاء الداخلية برمجياً.
    ```djazair
    app.onError(fn(err)
        print("خطأ في Webview: " + err)
    end)
    ```
*   `setDebug(enable)`: تفعيل أو تعطيل ميزات المطور أثناء وقت التشغيل.

---

## 3. إدارة النوافذ والأحداث

يمكنك الوصول للنافذة النشطة وإعدادها باستخدام الكائن `app.window`.

### التحكم البصري والمكاني:
*   `setTitle(text)` / `title()`: تعيين أو قراءة عنوان النافذة (يدعم اليونيكود Unicode بشكل كامل لجميع اللغات).
*   `setSize(width, height)` / `getSize()`: تعيين أو قراءة حجم النافذة `[العرض, الطول]`.
*   `setPosition(x, y)` / `getPosition()`: تعيين أو قراءة إحداثيات النافذة `[x, y]`.
*   `show()` / `hide()`: إظهار أو إخفاء النافذة.
*   `minimize()` / `maximize()` / `restore()`: تصغير، تكبير، أو استعادة النافذة لحجمها الأصلي.
*   `isMaximized()` / `isMinimized()` / `isVisible()`: التحقق من حالة النافذة الحالية.
*   `setBackgroundColor(r, g, b, a = 255)`: تعيين لون خلفية نافذة الويب (يمنع الوميض الأبيض أثناء تحميل الصفحات).
*   `setResizable(bool)`: تغيير قابلية النافذة لتعديل الحجم ديناميكياً.
*   `setMinimumSize(w, h)` / `setMaximumSize(w, h)`: وضع حدود قصوى ودنيا لأبعاد النافذة.

### التنقل وإدارة المحتوى:
*   `navigate(url)`: الانتقال إلى رابط إنترنت URL. عند تحميل ملف محلي (مثل `"index.html"`) يتم تعيينه بأمان كـ `http://djazair.local/index.html` لتجنب أخطاء حظر الـ CORS.
*   `setHtml(htmlString)`: تحميل كود HTML نصي مباشرة في المتصفح.
*   `eval(jsCode)`: تنفيذ كود JavaScript بشكل غير متزامن داخل المتصفح.
*   `initJs(jsCode)`: حقن وتنفيذ كود JavaScript قبل تحميل أي سكريبتات في مستند الويب.
*   `openDevTools()`: فتح نافذة فحص عناصر الويب (Developer Tools).
*   `reload()` / `goBack()` / `goForward()`: ضوابط إعادة التحميل والتحكم في سجل التنقل.
*   `canGoBack()` / `canGoForward()`: التحقق من توفر سجل تنقل للرجوع أو التقدم.
*   `setZoomLevel(level)` / `zoomLevel()`: تعيين أو قراءة مستوى التقريب (Zoom).
*   `setContextMenuEnabled(bool)`: تفعيل/تعطيل قائمة النقر بالزر الأيمن الافتراضية للمتصفح.

### أحداث النافذة (Window Events):
يمكنك الاستماع للأحداث عبر تمرير دوال كـ Callbacks:
*   `onClose(cb)`: يُستدعى عندما يحاول المستخدم إغلاق النافذة.
*   `onMove(cb)`: يُستدعى عند تحريك النافذة مع تمرير الإحداثيات: `fn(x, y)`.
*   `onResize(cb)`: يُستدعى عند تغيير حجم النافذة مع تمرير الأبعاد: `fn(width, height)`.
*   `onFocus(cb)` / `onBlur(cb)`: يُستدعى عند اكتساب أو فقدان النافذة للتركيز.
*   `onMaximize(cb)` / `onMinimize(cb)` / `onRestore(cb)`: يُستدعى عند تغيير حالة النافذة.
*   `onLoad(cb)`: يُستدعى بعد اكتمال تحميل المستند.
*   `onNavigate(url)`: يُستدعى عند بدء التنقل لرابط جديد.
*   `onTitleChange(title)`: يُستدعى عند تغيير عنوان صفحة الويب.

---

## 4. جسر الاتصال (IPC Bridge)

يعتبر الكائن `app.bridge` البوابة الرئيسية للتواصل بين الخلفية (Djazair) وواجهة المتصفح الأمامية.

### واجهة الـ JavaScript (Frontend API)
متاحة عالمياً تحت الكائن `window.djazair`:
*   `djazair.invoke(channel, data)`: استدعاء دالة مُسجلة في Djazair مع إرسال بيانات. يُرجع الـ JS `Promise` يحمل القيمة المعادة من الخلفية.
*   `djazair.on(channel, callback)`: الاستماع للأحداث المُرسلة من خلفية Djazair.
*   `djazair.send(channel, data)`: إرسال رسائل أو بيانات لقنوات الخلفية.

### واجهة الـ Djazair (Backend API)
*   `app.bridge.on(channel, handler)`: تسجيل مستمع لطلبات الواجهة الأمامية.
    ```djazair
    app.bridge.on("saveData", fn(payload)
        print("بيانات من المتصفح: " + str(payload))
        return "تم الحفظ!" # قيمة تُرسل لتُحل في الـ JS Promise
    end)
    ```
*   `app.bridge.send(channel, data)`: إرسال أحداث للمستمعين في الواجهة الأمامية.
    ```djazair
    app.bridge.send("notifyProgress", {"percentage": 80})
    ```
*   `app.bridge.expose(name, handler)`: كشف دالة من Djazair وربطها مباشرة بكائن `window` العام في المتصفح.
    ```djazair
    app.bridge.expose("add", fn(args)
        return args[0] + args[1]
    end)
    // In JS: window.add([5, 12]).then(sum => console.log(sum));
    ```

---

## 5. قوائم النظام وشريط المهام (Tray)

### القوائم المنبثقة (Context Menus)
يمكنك بناء قوائم تظهر عند النقر بالزر الأيمن أو عند الضغط على الأزرار:
*   `webview.menuCreate(label)`: إنشاء مقبض قائمة.
*   `webview.menuAddItem(menu, label, callback)`: إضافة عنصر قائمة مع دالة تُنفذ عند النقر عليه.
*   `webview.menuAddSeparator(menu)`: إدراج خط فاصل أُفقي.
*   `webview.menuCreateSubmenu(menu, label)`: إنشاء قائمة فرعية منبثقة.
*   `webview.menuPopup(windowHandle, menu)`: إظهار القائمة عند موضع الماوس الحالي. (يجب تمرير `app.window._handle` كمعامل أول).

### أيقونات شريط المهام (Tray API)
> ملاحظة: نظام הـ Tray يكشف حالياً الواجهة البرمجية لتكون متوافقة، ولكنه يعتمد على دوال وهمية (Stubs) في الـ C++. سيتم بناء نظام Win32 المكتمل لها في التحديثات القادمة.
*   `webview.trayCreate(label, iconPath)`
*   `webview.traySetIcon(tray, iconPath)`
*   `webview.traySetMenu(tray, menu)`
*   `webview.traySetTooltip(tray, text)`
*   `webview.trayShowBalloon(tray, title, message, timeout = 5)`
*   `webview.trayDestroy(tray)`

### الإشعارات النظامية (Toast Notifications)
إنشاء إشعارات نظام Windows 10/11 التي تظهر في مركز الإشعارات (Action Center):
*   `webview.notificationShow(title, message, subtitle = "", sound = False, icon = "", timeout = 5)`

---

## 6. نوافذ الحوار النظامية (Dialogs)

يمكنك الوصول لمربعات حوار نظام التشغيل الأصلية لفتح وحفظ الملفات بشكل آمن:

*   `webview.showMessageBox(options)`: عرض صندوق رسالة للمستخدم.
    *   *الخيارات:* `"type"` (info / warning / error), `"title"`, `"message"`, `"detail"`, `"buttons"` (ok / yesno / okcancel), `"defaultId"`.
*   `webview.showOpenFileDialog(options)`: نافذة اختيار وفتح ملف.
    *   *الخيارات:* `"title"`, `"defaultPath"`, `"filters"` (مثال `[{"name": "Docs", "extensions": ["txt", "json"]}]`), `"multiSelections"` (Bool).
    *   *تُرجع:* مسار الملف كنص، مصفوفة من المسارات في حال التحديد المتعدد، أو `Null` عند الإلغاء.
*   `webview.showSaveFileDialog(options)`: نافذة تحديد مسار لحفظ ملف جديد. تُرجع المسار أو `Null`.
*   `webview.showOpenFolderDialog(options)`: نافذة اختيار مجلد. تُرجع المسار أو `Null`.
*   `webview.showColorPicker(options)`: صندوق انتقاء الألوان الخاص بالنظام. تُرجع اللون المُختار.

---

## 7. مثال برمجي متكامل

```djazair
use webview
use file
use json

# 1. تهيئة التطبيق وتفعيل وضع التصحيح (Debug)
let app = webview.createWindow({
    "title": "Djazair Desktop Sandbox",
    "width": 900,
    "height": 700,
    "debug": True
})

# التقاط الأخطاء الداخلية
app.onError(fn(err)
    print("تم رصد خطأ: " + err)
end)

app.onReady(fn()
    print("التطبيق جاهز ويعمل الآن.")

    # 2. إنشاء قائمة منبثقة أصلية (Native Context Menu)
    let contextMenu = webview.menuCreate("Main Context Menu")
    webview.menuAddItem(contextMenu, "Trigger Native Toast", fn()
        webview.notificationShow("إشعار الويب", "تم الإطلاق من القائمة!", "Djazair Core", True)
    end)
    webview.menuAddSeparator(contextMenu)
    
    # إعداد قائمة فرعية للتحكم بالنافذة
    let windowSubMenu = webview.menuCreateSubmenu(contextMenu, "إدارة النافذة")
    webview.menuAddItem(windowSubMenu, "تصغير", fn() app.window.minimize() end)
    webview.menuAddItem(windowSubMenu, "تكبير", fn() app.window.maximize() end)
    webview.menuAddItem(windowSubMenu, "استعادة", fn() app.window.restore() end)
    
    webview.menuAddSeparator(contextMenu)
    webview.menuAddItem(contextMenu, "إغلاق التطبيق", fn() app.quit() end)

    # 3. التعامل مع قنوات الـ Bridge
    # إظهار القائمة المنبثقة عند النقر باليمين في المتصفح
    app.bridge.on("showContextMenu", fn(data)
        webview.menuPopup(app.window._handle, contextMenu)
        return True
    end)

    # نافذة اختيار ملف تُطلب من الـ JavaScript
    app.bridge.on("selectFile", fn(data)
        let filePath = webview.showOpenFileDialog({
            "title": "اختر مستنداً",
            "filters": [{"name": "Documents", "extensions": ["txt", "json", "dz"]}]
        })
        if !isNull(filePath)
            return filePath
        end
        return "لم يتم تحديد أي ملف"
    end)

    # نافذة حفظ ملف
    app.bridge.on("saveData", fn(payload)
        let savePath = webview.showSaveFileDialog({
            "title": "تصدير البيانات",
            "filters": [{"name": "JSON Document", "extensions": ["json"]}]
        })
        if !isNull(savePath)
            print("جاري حفظ البيانات في: " + savePath)
            file.write(savePath, json.stringify(payload))
            return "تم التصدير إلى " + savePath
        end
        return "تم إلغاء التصدير"
    end)

    # 4. تعيين واجهة المستخدم (HTML/CSS) الخاصة بالمتصفح
    app.window.setHtml(`
        <!DOCTYPE html>
        <html dir='rtl'>
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
                <h1>بيئة تجارب Djazair OS</h1>
                <p>انقر بالزر الأيمن في أي مكان لإظهار قائمة النظام الأصلية.</p>
                
                <button onclick='triggerOpenFile()'>فتح ملف</button>
                <button onclick='triggerSaveFile()'>تصدير البيانات</button>
                
                <div id='output'>جاهز. بانتظار أوامرك...</div>
            </div>
            
            <script>
                function triggerOpenFile() {
                    document.getElementById('output').innerText = 'جاري فتح النافذة...';
                    window.djazair.invoke('selectFile').then(path => {
                        document.getElementById('output').innerText = path;
                    });
                }
                
                function triggerSaveFile() {
                    document.getElementById('output').innerText = 'جاري الحفظ...';
                    window.djazair.invoke('saveData', {data: 'test'}).then(response => {
                        document.getElementById('output').innerText = response;
                    });
                }
            </script>
        </body>
        </html>
    `)
end)

# تشغيل حلقة الأحداث الأساسية
app.run()
```
