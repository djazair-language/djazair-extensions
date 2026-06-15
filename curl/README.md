# التوثيق الشامل لمكتبة Curl في لغة دزاير (Djazair Curl Extension Manual)

تعتبر إضافة **cURL** الموديول الرسمي والأساسي للاتصالات عبر الشبكة وإجراء طلبات HTTP/HTTPS في لغة دزاير. تعتمد هذه الإضافة على تغليف مكتبة **libcurl** العريقة، مما يمنحها موثوقية عالية وأداءً فائقاً مع دعم كامل للبروتوكولات الآمنة SSL، وإدارة الكوكيز (Cookies)، والتعامل مع خوادم الوكيل (Proxies)، ورفع الملفات، والمصادقة المتقدمة.

---

## 📋 جدول المحتويات
1. [التثبيت والتهيئة (Installation & Setup)](#1-التثبيت-والتهيئة-installation--setup)
2. [دليل الاستدعاء والبدء (Importing & Quick Start)](#2-دليل-الاستدعاء-والبدء-importing--quick-start)
3. [مرجع كلاس Curl التفصيلي (Class Reference)](#3-مرجع-كلاس-curl-التفصيلي-class-reference)
4. [دليل الثوابت الكامل (Constants Reference)](#4-دليل-الثوابت-الكامل-constants-reference)
5. [حلول المشاكل وإدارة الأمان (SSL & Windows Caveats)](#5-حلول-المشاكل-وإدارة-الأمان-ssl--windows-caveats)
6. [حالات استخدام متقدمة (Advanced Recipes)](#6-حالات-استخدام-متقدمة-advanced-recipes)
    - [تحميل ملف من الويب وحفظه محلياً](#أ-تحميل-ملف-من-الويب-وحفظه-محلياً)
    - [إرسال طلب POST مع ترويسات وبيانات JSON](#ب-إرسال-طلب-post-مع-ترويسات-وبيان-json)
    - [إرسال واستقبال الكوكيز (Cookies Management)](#ج-إرسال-واستقبال-الكوكيز-cookies-management)
    - [الاتصال عبر خادم وكيل (Proxy Connection)](#د-الاتصال-عبر-خادم-وكيل-proxy-connection)

---

## 1. التثبيت والتهيئة (Installation & Setup)

### التنزيل عبر مدير الحزم DPM
لتثبيت الإضافة في مشروعك، افتح ترمينال دزاير وشغّل الأمر:
```bash
dpm install curl
```

### البناء والتجميع (Compiling the DLL)
تتضمن الإضافة كود C أصلياً (`src/curl.c` و `src/curl_native.c`) يحتاج إلى ترجمة لإنتاج مكتبة الربط الديناميكي `curl.dll`. 
يمكنك تجميعها تلقائياً عبر:
```bash
dpm build curl
```
**المتطلبات الأساسية للبناء:**
1. وجود مترجم C (مثل `gcc` أو `clang`) مضافاً إلى متغيرات البيئة `PATH`.
2. تثبيت مكتبة `libcurl` على نظام التشغيل (في ويندوز، يُفضل استخدام حزم MSYS2/MinGW).

---

## 2. دليل الاستدعاء والبدء (Importing & Quick Start)

لاستيراد الإضافة، استخدم الكلمة المفتاحية `use`:
```djazair
use curl
```
بعد الاستيراد، يمكنك إنشاء كائن جديد من كلاس `Curl`:
```djazair
let client = curl.Curl()
```

---

## 3. مرجع كلاس Curl التفصيلي (Class Reference)

### كلاس `curl.Curl`
الكائن المسؤول عن تهيئة وإرسال واستقبال الطلبات.

#### `init()`
*   **الوصف:** تهيئ المقبض الأساسي لـ cURL (`easy handle`).
*   **نوع الإرجاع:** `void`.
*   **الأخطاء المحتملة:** ترمي استثناء `"CurlError"` إذا لم تكن مكتبة `libcurl` مثبتة أو غير قابلة للتحميل.

#### `setopt(option, value)`
*   **الوصف:** تعيين خيارات الطلب المحددة.
*   **المعاملات:**
    *   `option` (رقم): ثابت الخيار المراد تعيينه (انظر قسم الثوابت).
    *   `value` (نص أو رقم أو منطقي): القيمة المراد تمريرها للخيار.
*   **نوع الإرجاع:** `Bool` (`True` عند النجاح، `False` عند الفشل).
*   **تنبيه:** ترمي استثناء `ValueError` إذا كانت القيمة `Null`، أو `TypeError` إذا كانت القيمة من نوع غير مدعوم.

#### `setHeaders(headersList)`
*   **الوصف:** تعيين ترويسات الطلب (HTTP Headers).
*   **المعاملات:**
    *   `headersList` (مصفوفة): قائمة من النصوص بالصيغة `"Key: Value"`.
*   **نوع الإرجاع:** `Bool`.
*   **مثال:** `c.setHeaders(["Accept: application/json", "Authorization: Bearer my_token"])`

#### `perform()`
*   **الوصف:** تنفيذ طلب الاتصال بشكل متزامن.
*   **نوع الإرجاع:** `String` (يحتوي على جسم الاستجابة/الـ Response Body)، أو `Null` في حال فشل الاتصال.

#### `getInfo(info)`
*   **الوصف:** استرجاع معلومات الأداء والاتصال بعد اكتمال الطلب بنجاح.
*   **المعاملات:**
    *   `info` (رقم): ثابت المعلومة المطلوبة (مثل `curl.CURL_INFO_RESPONSE_CODE`).
*   **نوع الإرجاع:** `String` أو `Number` بحسب نوع المعلومة.

#### `getResponseHeaders()`
*   **الوصف:** جلب الترويسات التي أرسلها خادم الويب في استجابته للطلب الحالي.
*   **نوع الإرجاع:** `Map` (خريطة مفتاحها اسم الترويسة بالصيغة الصغيرة وقيمتها محتواها).

#### `getError()`
*   **الوصف:** جلب آخر رسالة خطأ بشرية مسجلة من محرك cURL عند فشل طلب الـ `perform()`.
*   **نوع الإرجاع:** `String`.

#### `cleanup()`
*   **الوصف:** إغلاق المقبض وتحرير موارد الذاكرة المخصصة للاتصال.
*   **نوع الإرجاع:** `void`.
*   **ملاحظة:** يجب استدعاء هذه الدالة دائماً في قسم `finally` لتجنب تسريب الذاكرة (Memory Leaks).

---

## 4. دليل الثوابت الكامل (Constants Reference)

توفر المكتبة مجموعة كبيرة من الثوابت المدمجة لإعداد خيارات الاتصال وجلب المعلومات:

### خيارات تعيين الطلب والمنافذ (`CURL_OPT_*`)
| الثابت | القيمة الرقمية | الوصف |
| :--- | :--- | :--- |
| `curl.CURL_OPT_URL` | `10002` | تعيين عنوان URL المستهدف. |
| `curl.CURL_OPT_USERAGENT` | `10018` | تحديد نص الـ User-Agent للطلب. |
| `curl.CURL_OPT_TIMEOUT` | `13` | الحد الأقصى لانتظار الطلب بالثواني. |
| `curl.CURL_OPT_CONNECTTIMEOUT` | `78` | وقت انتظار إنشاء الاتصال بالخادم بالثواني. |
| `curl.CURL_OPT_FOLLOWLOCATION` | `52` | تتبع التحويلات التلقائية (301, 302 redirects) إذا كانت `True`. |
| `curl.CURL_OPT_MAXREDIRS` | `68` | الحد الأقصى لمرات التحويل المتتابعة المسموح بها. |
| `curl.CURL_OPT_PROXY` | `10004` | عنوان خادم الوكيل (البروكسي) المطلوب العبور منه. |
| `curl.CURL_OPT_USERPWD` | `10005` | المصادقة الأساسية بالصيغة `"user:password"`. |

### خيارات الأمان والـ SSL
| الثابت | القيمة الرقمية | الوصف |
| :--- | :--- | :--- |
| `curl.CURL_OPT_SSL_VERIFYPEER` | `64` | التحقق من صحة شهادة SSL الخاصة بالخادم (`True` أو `False`). |
| `curl.CURL_OPT_SSL_VERIFYHOST` | `81` | التحقق من مطابقة المضيف للشهادة (قيمة `2` للتفعيل الكامل، `0` للإلغاء). |
| `curl.CURL_OPT_CAINFO` | `10065` | مسار ملف شهادات CA الموثوقة محلياً. |

### خيارات الـ Cookies وطرق الـ HTTP
| الثابت | القيمة الرقمية | الوصف |
| :--- | :--- | :--- |
| `curl.CURL_OPT_POST` | `47` | تحويل نوع الطلب إلى POST. |
| `curl.CURL_OPT_POSTFIELDS` | `10015` | البيانات المرفقة بطلب الـ POST. |
| `curl.CURL_OPT_CUSTOMREQUEST` | `10036` | طريقة مخصصة للطلب (مثال: `"PUT"`, `"DELETE"`, `"PATCH"`). |
| `curl.CURL_OPT_COOKIE` | `10022` | إرسال نصوص الكوكيز مباشرة في الترويسة. |
| `curl.CURL_OPT_COOKIEFILE` | `10031` | قراءة الكوكيز من ملف محلي محدد. |
| `curl.CURL_OPT_COOKIEJAR` | `10082` | كتابة الكوكيز الجديدة التي يرسلها الخادم إلى ملف محلي محدد. |

### معلومات جلب الأداء واستجابة الخادم (`CURL_INFO_*`)
| الثابت | القيمة الرقمية | الوصف |
| :--- | :--- | :--- |
| `curl.CURL_INFO_RESPONSE_CODE` | `2097154` | كود الاستجابة HTTP (مثال: 200, 404, 500). |
| `curl.CURL_INFO_CONTENT_TYPE` | `1048594` | نوع المحتوى المُعاد (Content-Type header). |
| `curl.CURL_INFO_TOTAL_TIME` | `3145731` | الوقت الإجمالي بالثواني المستغرق في تنفيذ الاتصال. |
| `curl.CURL_INFO_SIZE_DOWNLOAD` | `3145736` | حجم البيانات المحملة بالبايت. |

---

## 5. حلول المشاكل وإدارة الأمان (SSL & Windows Caveats)

### مشكلة شهادات الأمان (SSL Trust Anchors) على نظام ويندوز:
على أنظمة تشغيل ويندوز، قد يفشل الطلب ويرمي خطأ شبيه بـ:
`error adding trust anchors from file: .../ca-bundle.crt`
يحدث هذا لأن نسخة libcurl لا تملك إمكانية الوصول التلقائي لمخزن شهادات الأمان لنظام التشغيل.

#### **الحل الأول (الآمن للإنتاج):**
تحميل ملف الشهادات المعتمد `cacert.pem` من الموقع الرسمي لـ cURL وتعيينه للاتصال:
```djazair
c.setopt(curl.CURL_OPT_CAINFO, "C:/path/to/cacert.pem")
```

#### **الحل الثاني (للتطوير والاختبار المحلي فقط):**
تعطيل التحقق من شهادة الـ SSL لحل المشكلة فوراً (تنبيه: لا تستخدم هذا في بيئة الإنتاج الفعلي):
```djazair
c.setopt(curl.CURL_OPT_SSL_VERIFYPEER, False)
c.setopt(curl.CURL_OPT_SSL_VERIFYHOST, False)
```

---

## 6. حالات استخدام متقدمة (Advanced Recipes)

### أ. تحميل ملف من الويب وحفظه محلياً
يوضح هذا المثال كيفية جلب ملف صورة من الويب وكتابتها في ملف محلي باستخدام موديول `file` القياسي:
```djazair
use curl
use file

let c = curl.Curl()
try
    c.setopt(curl.CURL_OPT_URL, "https://httpbin.org/image/png")
    c.setopt(curl.CURL_OPT_SSL_VERIFYPEER, False) # تجاوز شهادة SSL مؤقتاً
    c.setopt(curl.CURL_OPT_SSL_VERIFYHOST, False)
    
    # تنفيذ الطلب
    let imgData = c.perform()
    if !isNull(imgData) and c.getInfo(curl.CURL_INFO_RESPONSE_CODE) == 200
        # حفظ كملف ثنائي
        file.write("downloaded_image.png", imgData)
        print("Image downloaded and saved successfully!")
    else
        print("Failed to download image: " + c.getError())
    end
catch e
    print("Error: " + str(e))
finally
    c.cleanup()
end
```

### ب. إرسال طلب POST مع ترويسات وبيانات JSON
```djazair
use curl

let c = curl.Curl()
try
    c.setopt(curl.CURL_OPT_URL, "https://httpbin.org/post")
    c.setopt(curl.CURL_OPT_POST, True)
    c.setopt(curl.CURL_OPT_SSL_VERIFYPEER, False)
    c.setopt(curl.CURL_OPT_SSL_VERIFYHOST, False)
    
    # تعيين الترويسات والبيانات
    c.setHeaders([
        "Content-Type: application/json",
        "Accept: application/json"
    ])
    c.setopt(curl.CURL_OPT_POSTFIELDS, "{\"title\": \"Djazair Tutorial\", \"completed\": false}")
    
    let response = c.perform()
    if !isNull(response)
        print("HTTP Status: " + str(c.getInfo(curl.CURL_INFO_RESPONSE_CODE)))
        print("Response Body: " + response)
    else
        print("Failed: " + c.getError())
    end
catch e
    print("Error: " + str(e))
finally
    c.cleanup()
end
```

### ج. إرسال واستقبال الكوكيز (Cookies Management)
مفيد جداً عند محاكاة تسجيل الدخول أو الحفاظ على الجلسة النشطة بين الطلبات المتتالية:
```djazair
use curl

let c = curl.Curl()
try
    c.setopt(curl.CURL_OPT_URL, "https://httpbin.org/cookies/set?session_token=dz123456")
    c.setopt(curl.CURL_OPT_SSL_VERIFYPEER, False)
    c.setopt(curl.CURL_OPT_SSL_VERIFYHOST, False)
    
    # تحديد ملف حفظ الكوكيز المستقبلة وقراءتها مجدداً
    c.setopt(curl.CURL_OPT_COOKIEJAR, "./cookies.txt")
    c.setopt(curl.CURL_OPT_COOKIEFILE, "./cookies.txt")
    
    let res = c.perform()
    print("Set Cookie Response: " + res)
    
    # الطلب الثاني: سيقوم تلقائياً بإرسال الكوكيز المحفوظة في الملف
    c.setopt(curl.CURL_OPT_URL, "https://httpbin.org/cookies")
    let res2 = c.perform()
    print("Sent Cookies Verified By Server: " + res2)
catch e
    print("Error: " + str(e))
finally
    c.cleanup()
end
```

### د. الاتصال عبر خادم وكيل (Proxy Connection)
لإجبار كافّة الطلبات على العبور من خلال بروكسي محدد:
```djazair
use curl

let c = curl.Curl()
try
    c.setopt(curl.CURL_OPT_URL, "https://httpbin.org/get")
    c.setopt(curl.CURL_OPT_SSL_VERIFYPEER, False)
    c.setopt(curl.CURL_OPT_SSL_VERIFYHOST, False)
    
    # تعيين عنوان الوكيل والمنفذ
    c.setopt(curl.CURL_OPT_PROXY, "http://127.0.0.1:8080")
    
    # تعيين بيانات المصادقة للبروكسي إذا تطلب الأمر
    # c.setopt(curl.CURL_OPT_PROXYUSERPWD, "user:pass")
    
    let body = c.perform()
    if !isNull(body)
        print("Fetched via Proxy successfully!")
    else
        print("Proxy failed: " + c.getError())
    end
catch e
    print("Error: " + str(e))
finally
    c.cleanup()
end
```
