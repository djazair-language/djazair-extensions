# إضافة SQLite لغة دزاير (SQLite Extension for Djazair)

توفر هذه الإضافة واجهة بسيطة وفعالة للتعامل مع قواعد بيانات SQLite المحلية، مع دعم كامل للعمليات المتزامنة وحماية مدمجة من ثغرات حقن الاستعلامات (SQL Injection).

---

## كيفية الاستدعاء (Importing)
يتم استيراد الحزمة مباشرةً باستخدام:
```djazair
use sqlite
```

أما في حالة الحاجة للتعامل المباشر مع واجهة الـ C البرمجية منخفضة المستوى (الـ API الأصلي)، يمكن استدعاء موديول النظام:
```djazair
let _sqlite = __native("_sqlite")
```

---

## كلاس `Connection`
يُستخدم لإدارة وفتح ملفات قواعد البيانات وتأمين الاستعلامات.

### الدوال البرمجية (Methods)
| الدالة | الوصف |
| :--- | :--- |
| `connect(path)` | فتح ملف قاعدة البيانات (يتم إنشاء الملف تلقائياً إذا لم يكن متواجداً). |
| `isConnected()` | التحقق من نجاح فتح قاعدة البيانات وصحتها. |
| `query(sql)` | تنفيذ استعلام `SELECT` وإرجاع كائن من كلاس `ResultSet`. |
| `execute(sql)` | تنفيذ عمليات تعديل الهيكل أو تحديث البيانات (`CREATE`, `INSERT`, `UPDATE`, `DELETE`). |
| `safeQuery(sql, params)` | تنفيذ استعلام آمن وقائي باستخدام علامات الاستفهام `?` والبارامترات لمنع حقن SQL. |
| `safeExecute(sql, params)` | تنفيذ عملية تعديل آمنة ووقائية ممررة بالبارامترات باستخدام علامات الاستفهام `?`. |
| `changes()` | إرجاع عدد الصفوف المتأثرة بآخر عملية إدخال أو تعديل. |
| `insertId()` | إرجاع معرف الصف الأخير المدرج (`last_insert_rowid`). |
| `beginTransaction()` | بدء معاملة برمجية نشطة (Transaction). |
| `commit()` | تثبيت التغييرات الحالية وتخزينها نهائياً. |
| `rollback()` | التراجع الكامل عن التغييرات الحالية والرجوع للحالة السابقة. |
| `escape(val)` | معالجة النصوص وحمايتها يدوياً. |
| `error()` | جلب آخر رسالة خطأ مسجلة من المحرك. |
| `raw()` | استرجاع المورد البرمجي لـ C المباشر لقاعدة البيانات (`SQLiteDB Resource`). |
| `close()` | إغلاق الاتصال وتحرير الموارد بأمان. |

---

## كلاس `ResultSet`
لإدارة والتحكم بنتائج البحث والاستعلامات واستخلاص البيانات.

### الدوال البرمجية (Methods)
| الدالة | الوصف |
| :--- | :--- |
| `fetchRow()` | جلب الصف التالي من النتائج كمصفوفة عادية (`Array`). |
| `fetchAssoc()` | جلب الصف التالي كجدول ترابطي/خريطة (`Map`) مفتاحها اسم العمود وقيمتها محتوى الحقل. |
| `fetchAllAssoc()` | جلب كافة الصفوف المتبقية دفعة واحدة كقائمة من الخرائط (`Array of Maps`). |
| `columns()` | إرجاع مصفوفة بأسماء كافة الأعمدة في نتيجة الاستعلام الحالية. |
| `raw()` | استرجاع مورد التعليمة البرمجية لـ C الأصلي (`SQLiteStmt Resource`). |
| `close()` | إغلاق الاستعلام الجاري وتحرير الذاكرة المخصصة له. |

---

## الثوابت (Constants)

| الثابت | القيمة | الوصف |
|--------|--------|-------|
| `sqlite.SQLITE_ROW` | `100` | دالة `_s.step()` أعادت صفاً — توجد نتائج للقراءة |
| `sqlite.SQLITE_DONE` | `101` | دالة `_s.step()` أعادت الانتهاء — لا مزيد من النتائج |

> **ملاحظة:** هذه الثوابت مفيدة عند استخدام واجهة `__native("_sqlite")` مباشرة للتحكم المنخفض المستوى.

---

## مثال تطبيقي متكامل (Complete Example)
```djazair
use sqlite

# 1. الاتصال بقاعدة البيانات
let db = sqlite.connect("school.db")

if db.isConnected():
    # 2. إنشاء الجداول
    db.execute("CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY, name TEXT, score REAL)")
    
    # 3. إدخال بيانات آمن ومحمي
    db.safeExecute("INSERT INTO users (name, score) VALUES (?, ?)", ["Riad", 98.5])
    db.safeExecute("INSERT INTO users (name, score) VALUES (?, ?)", ["Anis", 89.0])
    
    # 4. استعلام آمن مفلتر بالبارامترات
    let res = db.safeQuery("SELECT * FROM users WHERE score > ?", [90.0])
    
    # طباعة الأعمدة
    print("Columns: " + str(res.columns()))
    
    # قراءة البيانات كخرائط
    let users = res.fetchAllAssoc()
    let i = 0
    while i < users.length():
        let user = users[i]
        print("User [ID: " + str(user["id"]) + ", Name: " + user["name"] + ", Score: " + str(user["score"]) + "]")
        i += 1
    end
    
    # تحرير الموارد
    res.close()
    db.close()
    print("Process finished successfully.")
else:
    print("Failed to connect to database: " + db.error())
end
```
