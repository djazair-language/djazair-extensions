# التوثيق الشامل لإطار عمل الويب دزاير (Djazair `dweb` Web Framework Manual)

يعتبر **dweb** إطار عمل ويب متكامل وقوي مبني بالكامل بلغة دزاير، ومصمم لتسهيل بناء تطبيقات ومواقع الويب وخوادم الويب (APIs) بنمط **MVC (Model-View-Controller)**. يتميز إطار العمل بأدائه العالي وحمايته المدمجة، ويوفر بيئة برمجية متطورة تشمل نظام مسارات مرناً (Routing)، ومحرك قوالب AST متقدماً (Templates)، ومصادقة بالاعتماد على الجلسات (Sessions)، وحماية تلقائية ضد ثغرات CSRF، ومستلم ملفات مرفوعة، وباني استعلامات مرناً لقواعد البيانات (QueryBuilder).

---

## 📋 جدول المحتويات
1. [هيكل المشروع والتهيئة (Directory Structure & Setup)](#1-هيكل-المشروع-والتهيئة-directory-structure--setup)
2. [المسارات والتحكم (Routing & Controllers)](#2-المسارات-والتحكم-routing--controllers)
3. [التحقق وفحص البيانات (Validator API)](#3-التحقق-وفحص-البيانات-validator-api)
4. [محرك القوالب والتصميم (AST Template Engine)](#4-محرك-القوالب-والتصميم-ast-template-engine)
5. [باني استعلامات قواعد البيانات (QueryBuilder & DB)](#5-باني-استعلامات-قواعد-البيانات-querybuilder--db)
6. [إدارة الجلسات والأمان (Sessions & CSRF Protection)](#6-إدارة-الجلسات-والأمان-sessions--csrf-protection)
7. [معالجة رفع الملفات (Multipart File Upload)](#7-معالجة-رفع-الملفات-multipart-file-upload)
8. [مشروع ويب متكامل (Complete Web MVC App)](#8-مشروع-ويب-متكامل-complete-web-mvc-app)

---

## 1. هيكل المشروع والتهيئة (Directory Structure & Setup)

لإنتاج تطبيق ويب متناسق ونظيف بنمط MVC، يوصى بالهيكل التالي للمجلدات:
```text
my-web-app/
├── app.dz                # ملف الدخول الرئيسي
├── controllers/          # وحدات التحكم بالطلب
│   └── UserController.dz
├── models/               # هياكل البيانات والاتصال بالـ DB
├── views/                # قوالب العرض (HTML)
│   ├── layouts/
│   │   └── base.html     # التصميم العام للموقع
│   ├── index.html        # الصفحة الرئيسية
│   └── register.html     # صفحة التسجيل
└── public/               # الملفات الثابتة المتاحة للعموم
    ├── css/
    ├── js/
    └── uploads/          # مجلد رفع صور الأعضاء
```

### التثبيت عبر DPM:
```bash
dpm install dweb
```

---

## 2. المسارات والتحكم (Routing & Controllers)

### كلاس التطبيق الرئيسي `dweb.App`
يتحكم في استقبال طلبات HTTP وإدارة المسارات والبرمجيات الوسيطة (Middlewares).

*   `init(options)`: إنشاء كائن التطبيق.
    *   *خيارات:* `"views"` (مسار مجلد العروض، الافتراضي هو مجلد السكربت المنفّذ).
*   `setViews(directory)`: تعيين أو تعديل مسار قوالب العرض ديناميكياً.
*   `middleware(handler)`: تسجيل برمجية وسيطة عالمية تُنفذ على جميع الطلبات بالصيغة `fn(req, res, next)`.
*   `serveStatic(url_prefix, directory_path)`: خدمة الملفات الثابتة من مجلد محدد (مثال: `app.serveStatic("/assets", "./public")`).
*   `run(port = 8080)`: بدء تشغيل خادم الويب والاستماع للمنفذ المحدد.

### دوال المسارات (Routing Methods):
تأخذ المعاملات: `path` (المسار المطلوب)، `handler` (دالة المعالجة أو كلاس التحكم)، و `middlewares` (مصفوفة برمجيات وسيطة اختيارية خاصة بالمسار).
*   `get(path, handler, middlewares = [])`
*   `post(path, handler, middlewares = [])`
*   `put(path, handler, middlewares = [])`
*   `patch(path, handler, middlewares = [])`
*   `delete(path, handler, middlewares = [])`
*   `group(prefix, callback)`: تجميع عدة مسارات ببادئة مشتركة.

### كلاس الطلب `Request` (الكائن `req`)
يحتوي على تفاصيل الطلب المرسل من المتصفح:
*   `req.method`: طريقة الطلب (GET, POST...).
*   `req.path`: المسار المطلوب.
*   `req.headers`: خريطة بالترويسات المستقبلة.
*   `req.cookies`: خريطة بالكوكيز المرسلة من المتصفح.
*   `req.query`: خريطة بمعاملات رابط الطلب (Query string).
*   `req.body`: خريطة ببيانات الطلب المرسلة عبر النماذج (Forms) أو الـ JSON.
*   `req.pathParams`: خريطة ببارامترات المسار المتغير (مثال: للمسار `"/users/:id"`، قيمة `req.pathParams["id"]` ستعود بمعرف المستخدم).
*   `req.session`: كائن الجلسة الفردي للمستخدم (متاح فقط عند تفعيل برمجية الجلسات).

### كلاس الاستجابة `Response` (الكائن `res`)
يتحكم في تشكيل وإرسال الرد للمتصفح:
*   `res.status(code)`: تعيين رمز الحالة (HTTP Status code). يرجع نفس كائن الاستجابة لتمكين الاستدعاء المتتالي.
*   `res.header(name, value)`: تعيين ترويسة معينة للاستجابة.
*   `res.cookie(name, value, options = {})`: إرسال كوكيز للمتصفح.
*   `res.send(body)`: إرسال رد نصي عادي أو خام للمتصفح.
*   `res.html(htmlString)`: إرسال كود HTML للمتصفح وتعيين الترويسة `Content-Type: text/html`.
*   `res.json(data)`: إرسال كود JSON وتعيين الترويسة `Content-Type: application/json`.
*   `res.redirect(url)`: إعادة توجيه المتصفح لصفحة أخرى.
*   `res.view(templatePath, data = {})`: قراءة وعرض قالب محدد وتمرير خريطة بالبيانات له لمعالجتها.

---

## 3. التحقق وفحص البيانات (Validator API)

يوفر كلاس `dweb.Validator` واجهة مرنة وسلسة للتحقق من صحة المدخلات وخلوها من الأخطاء:

```djazair
let v = dweb.Validator(req.body)
    .required("username").minLength("username", 4)
    .required("email").email("email")
    .optional("website").url("website")
    .required("role").oneOf("role", ["admin", "member"])
```

### قواعد الفحص المتاحة (Validation Rules):
*   `required(field)`: يجب أن يكون الحقل متواجداً وغير فارغ.
*   `optional(field)`: الحقل اختياري (تجاوز بقية القواعد إذا لم يكن متواجداً).
*   `string(field)`: يجب أن يكون الحقل نصاً.
*   `number(field)`: يجب أن يكون الحقل عدداً.
*   `boolean(field)`: يجب أن يكون الحقل قيمة منطقية (1, 0, true, false).
*   `minLength(field, length)` / `maxLength(field, length)`: الحد الأدنى والأقصى لطول النص.
*   `min(field, value)` / `max(field, value)`: القيمة الرقمية الصغرى والكبرى للعداد.
*   `email(field)`: فحص مطابقة صيغة البريد الإلكتروني.
*   `url(field)`: فحص مطابقة صيغة رابط الويب (يبدأ بـ `http://` أو `https://`).
*   `oneOf(field, arrayChoices)`: يجب أن تكون القيمة أحد الخيارات المدرجة بالمصفوفة.
*   `notOneOf(field, arrayChoices)`: يجب ألا تكون القيمة ضمن الخيارات الممنوعة.
*   `same(field, otherField)`: يجب مطابقة محتوى الحقلين تماماً (مثل حقل كلمة المرور وتأكيدها).
*   `different(field, otherField)`: يجب اختلاف محتوى الحقلين.
*   `custom(field, rule_fn)`: تمرير دالة فحص مخصصة `rule_fn(val)` ترجع `True` عند الصحة أو نص رسالة الخطأ عند الفشل.

### دوال النتائج والتحقق (`Validator` results):
*   `passes()`: ترجع `True` في حال خلو المدخلات من أخطاء التحقق.
*   `fails()`: ترجع `True` في حال وجود خطأ تحقق واحد على الأقل.
*   `errors()`: ترجع خريطة بالأخطاء المسجلة بالصيغة `{ "field": ["error_message"] }`.
*   `firstError(field)`: جلب أول رسالة خطأ للحقل المحدد.
*   `validated()`: إرجاع البيانات الآمنة التي تم فحصها ونجحت فقط (لاستخدامها مباشرةً في قاعدة البيانات وتجنب حقن مدخلات ضارة).

---

## 4. محرك القوالب والتصميم (AST Template Engine)

يعمل محرك القوالب المدمج عبر قراءة ملفات HTML من مجلد العروض، وتحويلها لـ AST (شجرة قواعد تمثيلية) ومعالجتها برمجياً للتSubstitution والتحكم.

### الميزات البرمجية للقوالب:

#### أ. التوريث والتجزئة (Inheritance & Partials):
يمكنك مشاركة تصميم أساسي موحد وتعديل أقسام معينة لكل صفحة.
في الملف الرائد `views/layouts/base.html`:
```html
<!DOCTYPE html>
<html>
<head>
    <title>{% block title %}Default Title{% endblock %}</title>
</head>
<body>
    {% include "partials/nav.html" %}  <!-- تضمين شريط التنقل -->
    
    <main>
        {% block content %}{% endblock %} <!-- مكان حقن محتوى الصفحة الفرعية -->
    </main>
</body>
</html>
```
وفي صفحة العرض الخاصة بالصفحة الرئيسية `views/index.html`:
```html
{% extends "layouts/base.html" %}

{% block title %}My Home Page{% endblock %}

{% block content %}
    <h1>Welcome Home!</h1>
    <p>This content is injected dynamically.</p>
{% endblock %}
```

#### ب. الجمل الشرطية والتحكم:
```html
{% if user.role == "admin" %}
    <p>Welcome Admin!</p>
{% elif user.role == "member" %}
    <p>Welcome Member!</p>
{% else %}
    <p>Welcome Guest!</p>
{% endif %}
```

#### ج. الحلقات والتكرار:
يمرر المتغيران بالترتيب (معامل الفهرس، الكائن المأخوذ):
```html
<ul>
    {% for index, u in users %}
        <li>#{{ index + 1 }} - Name: {{ u.name }} - Email: {{ u.email }}</li>
    {% endfor %}
</ul>
```

#### د. حماية الـ XSS وطباعة البيانات:
*   `{{ text }}`: ستقوم بتصفية رموز HTML تلقائياً وطباعتها بشكل آمن (مثال: تحويل `<` لـ `&lt;`).
*   `{{{ trusted_html }}}`: طباعة النص الخام بالكامل دون تصفية (مفيد لتضمين كود محرر نصوص منسق).

---

## 5. باني استعلامات قواعد البيانات (QueryBuilder & DB)

يدعم إطار العمل اتصالاً ذكياً كسولاً (Lazy) بالـ SQLite أو MySQL؛ حيث يتم تهيئة قواعد البيانات ولكن لا يتم الاتصال الفعلي بالخوادم إلا عند تنفيذ أول استعلام لتقليل زمن التحميل وموارد النظام.

### باني الاستعلامات `QueryBuilder`
يسهل صياغة استعلامات قواعد البيانات بلغة دزاير محمية تماماً ضد ثغرات حقن الاستعلامات (SQL Injection) عن طريق إفلات البارامترات يدوياً وتلقائياً.

#### استعلامات القراءة (Select & Fetch):
*   `app.table(db, "name")`: تحديد الجدول المطلوب العمل عليه.
*   `select(arrayColumns)`: اختيار أعمدة معينة (الافتراضي اختيار الكل `*`).
*   `where(field, value)` أو `where(field, operator, value)`: إضافة تصفية شرطية (مثال: `where("age", ">=", 18)`).
*   `whereIn(field, arrayValues)`: شرط تواجد القيمة ضمن مصفوفة خيارات.
*   `orderBy(field, direction = "ASC")`: ترتيب النتائج تصاعدياً أو تنازلياً.
*   `limit(number)` / `offset(number)`: تحديد حجم النتائج والإزاحة لعمل تصفح الصفحات.
*   `get()`: تنفيذ الاستعلام وإرجاع مصفوفة من الخرائط بالنتائج كاملة.
*   `first()`: تنفيذ الاستعلام وجلب الصف الأول فقط كخريطة `Map` أو `Null`.
*   `count()`: إرجاع عدد الصفوف المطابقة للشرط.
*   `exists()`: التحقق من وجود نتائج تطابق الشرط (ترجع `True` أو `False`).
*   `paginate(page, perPage)`: تصفح متطور للبيانات يرجع خريطة تحتوي على النتائج والعدد الإجمالي وبيانات الصفحة الحالية.

#### استعلامات التعديل (Write Operations):
*   `insert(mapData)`: إدخال صف جديد. تأخذ خريطة بالبيانات وترجع المعرف المدرج الأخير (`insertId`).
*   `update(mapData)`: تعديل البيانات للصفوف التي تطابق شروط الـ `where()` المسبقة. ترجع `True` أو `False`.
*   `delete()`: حذف الصفوف التي تطابق شروط الـ `where()` المسبقة. ترجع `True` أو `False`.

---

## 6. إدارة الجلسات والأمان (Sessions & CSRF Protection)

### كود الجلسات (`app.sessions(options)`)
برمجية وسيطة تقوم بتثبيت كوكيز مشفر لحفظ بيانات الجلسة لكل متصفح.
*   *خيارات:* `"secret"` (مفتاح تشفير الكوكيز، يجب أن يكون فريداً وقوياً)، `"lifetime"` (عمر الكوكيز بالثواني).
*   **الدوال المتاحة عبر `req.session`:**
    *   `set(key, value)` / `get(key, defaultValue)`: حفظ وجلب قيم الجلسة.
    *   `remove(key)`: حذف مفتاح محدد من الجلسة.
    *   `setFlash(key, message)`: حفظ رسالة سريعة تُعرض لمرة واحدة في الطلب القادم ثم تُحذف تلقائياً من الذاكرة (مفيدة لرسائل تأكيد العمليات أو أخطاء النماذج).
    *   `getFlash(key)`: استرجاع رسالة الـ Flash.

### حماية CSRF المدمجة
عند تفعيل البرمجية الوسيطة `app.middleware(app.csrf())`:
1.  يقوم الخادم بتوليد توكن فريد للجلسة وحفظه في `req.csrfToken`.
2.  يجب إدراج هذا التوكن في أي نموذج يقوم بتعديل البيانات (POST/PUT/PATCH/DELETE) كحقل مخفي باسم `_csrf` أو إرساله في الترويسات كـ `X-CSRF-Token` في طلبات الأجاكس.
3.  إذا لم يطابق التوكن المرسل ما تم حفظه في الجلسة، سيعيد الخادم صفحة خطأ `419 - CSRF Token Mismatch`.

---

## 7. معالجة رفع الملفات (Multipart File Upload)

يوفر كلاس `dweb.FileUpload` استقبالاً آمناً للملفات المرفوعة من النماذج التي تستخدم `enctype="multipart/form-data"`.

*   `FileUpload(req, options)`: إعداد مستلم الملفات.
    *   *خيارات:* `"dest"` (مجلد حفظ الملف المرفوع)، `"maxSize"` (أقصى حجم مسموح للبايت)، `"allowed"` (مصفوفة الامتدادات المسموح بها مثل `[".png", ".jpg"]`).
*   `upload.file(fieldName)`: جلب الكائن الممثل للملف المرفوع عبر اسم الحقل في النموذج.
*   `file.isValid()`: التحقق من مطابقة الملف للشروط والقيود (الحجم والامتداد).
*   `file.save(customName)`: حفظ الملف في مجلد الوجهة. إذا لم يتم تمرير اسم مخصص، فسيقوم النظام بتوليد اسم فريد بالاعتماد على التوقيت الزمني لمنع تكرار المسميات.
*   `file.savedPath`: جلب المسار الكامل لحفظ الملف بعد الانتهاء.
*   `file.error`: جلب رسالة خطأ التحقق للملف عند الفشل.

---

## 8. مشروع ويب متكامل (Complete Web MVC App)

مثال برمجي كامل يربط قاعدة البيانات والجلسات وحماية الـ CSRF والتحقق والقوالب لعرض وإضافة أعضاء:

```djazair
use dweb
use sqlite

# 1. تهيئة التطبيق وإعداد مجلد العروض وقواعد البيانات الكسولة
let app = dweb.App({
    "views": "./views"
})

# الاتصال بقاعدة بيانات SQLite
let db = app.sqlite("portal.db")
db.execute("CREATE TABLE IF NOT EXISTS members (id INTEGER PRIMARY KEY AUTO_INCREMENT, name TEXT, email TEXT, avatar TEXT)")

# 2. تفعيل البرمجيات الوسيطة الأساسية
app.middleware(app.sessions({
    "secret": "djazair_portal_secure_passphrase"
}))
app.middleware(app.csrf())

# 3. مسار GET لعرض الواجهة الرسومية والأعضاء
app.get("/", fn(req, res)
    # جلب الأعضاء عبر باني الاستعلامات
    let members = app.table(db, "members").orderBy("id", "DESC").get()
    
    # جلب رسائل الـ Flash
    let successMessage = req.session.getFlash("success")
    let errorsMap = req.session.getFlash("errors")
    
    res.view("index.html", {
        "members": members,
        "success": successMessage,
        "errors": errorsMap,
        "csrf_token": req.csrfToken
    })
end)

# 4. مسار POST لاستقبال النموذج والتحقق ورفع الملف
app.post("/register", fn(req, res)
    # أ. التحقق من صحة المدخلات النصية
    let validator = dweb.Validator(req.body)
        .required("name").minLength("name", 3).maxLength("name", 50)
        .required("email").email("email")
        
    if validator.fails()
        req.session.setFlash("errors", validator.errors())
        res.redirect("/")
        return
    end

    # ب. معالجة رفع الملف المرفق
    let upload = dweb.FileUpload(req, {
        "dest": "./public/uploads",
        "maxSize": 3 * 1024 * 1024, # 3 Megabytes
        "allowed": [".png", ".jpg", ".jpeg"]
    })
    
    let avatarFile = upload.file("avatar")
    let avatarPath = "/uploads/default.png"
    
    if avatarFile.isValid()
        avatarFile.save()
        # جلب اسم الملف بعد الحفظ الآمن
        avatarPath = "/uploads/" + path.basename(avatarFile.savedPath)
    else
        # إذا وجد ملف وحجمه غير صالح أو امتداده غير مسموح
        if avatarFile.error != "No file provided for field 'avatar'"
            req.session.setFlash("errors", {"avatar": [avatarFile.error]})
            res.redirect("/")
            return
        end
    end

    # ج. إدخال البيانات المعتمدة بأمان لقاعدة البيانات
    let validated = validator.validated()
    app.table(db, "members").insert({
        "name": validated["name"],
        "email": validated["email"],
        "avatar": avatarPath
    })

    req.session.setFlash("success", "Member added successfully!")
    res.redirect("/")
end)

# 5. تفعيل تقديم الصور والملفات المرفوعة للجمهور
app.serveStatic("/uploads", "./public/uploads")

# 6. تشغيل الخادم
app.run(8080)
```

### القالب الموحد للتطبيق (`views/index.html`):
```html
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>Djazair Portal Management</title>
    <style>
        body { font-family: 'Segoe UI', sans-serif; background: #0c0c0e; color: #f1f5f9; max-width: 650px; margin: 40px auto; padding: 20px; }
        .card { background: #18181b; border: 1px solid #27272a; padding: 20px; border-radius: 10px; margin-bottom: 15px; display: flex; align-items: center; }
        .avatar { width: 55px; height: 55px; border-radius: 50%; border: 2px solid #8b5cf6; margin-right: 15px; object-fit: cover; }
        input, button { padding: 12px; margin: 8px 0; display: block; width: 100%; box-sizing: border-box; border-radius: 6px; }
        input { background: #18181b; border: 1px solid #27272a; color: white; }
        button { background: linear-gradient(135deg, #8b5cf6, #3b82f6); color: white; border: none; font-weight: bold; cursor: pointer; }
        .alert-success { padding: 15px; background: rgba(16, 185, 129, 0.1); border: 1px solid #10b981; color: #10b981; border-radius: 6px; margin-bottom: 15px; }
        .error-text { color: #ef4444; font-size: 13px; margin-top: -5px; margin-bottom: 10px; display: block; }
    </style>
</head>
<body>
    <h1>Add New Member</h1>
    
    {% if success %}
        <div class="alert-success">{{ success }}</div>
    {% endif %}

    <form method="POST" action="/register" enctype="multipart/form-data">
        <!-- توكن الأمان CSRF -->
        <input type="hidden" name="_csrf" value="{{ csrf_token }}">
        
        <label>Full Name:</label>
        <input type="text" name="name" placeholder="Enter name">
        {% if errors.name %}
            <span class="error-text">{{ errors.name[0] }}</span>
        {% endif %}
        
        <label>Email Address:</label>
        <input type="email" name="email" placeholder="name@domain.com">
        {% if errors.email %}
            <span class="error-text">{{ errors.email[0] }}</span>
        {% endif %}
        
        <label>Profile Picture:</label>
        <input type="file" name="avatar">
        {% if errors.avatar %}
            <span class="error-text">{{ errors.avatar[0] }}</span>
        {% endif %}
        
        <button type="submit">Save Member</button>
    </form>

    <hr style="border: 0; border-top: 1px solid #27272a; margin: 30px 0;">

    <h2>Registered Members ({{ members.length() }})</h2>
    {% for index, member in members %}
        <div class="card">
            <img class="avatar" src="{{ member.avatar }}" alt="User Avatar">
            <div>
                <strong style="font-size: 1.1rem; color: white;">{{ member.name }}</strong><br>
                <span style="color: #94a3b8; font-size: 0.9rem;">{{ member.email }}</span>
            </div>
        </div>
    {% endfor %}
</body>
</html>
```
