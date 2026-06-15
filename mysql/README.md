# إضافة MySQL لغة دزاير (MySQL Extension for Djazair)

توفر هذه الإضافة واجهة للتعامل مع قواعد بيانات MySQL مع دعم الاتصال الآمن، الاستعلامات المحمية من حقن SQL، وإدارة المعاملات (Transactions).

---

## كيفية الاستدعاء (Importing)

```djazair
use mysql
```

---

## كلاس `Connection`

يُستخدم لإدارة الاتصال بقاعدة بيانات MySQL.

### `init(host, user, password, db, port)`

إنشاء اتصال جديد بقاعدة البيانات.

| المعامل | النوع | الوصف |
|---------|-------|-------|
| `host` | String | مضيف قاعدة البيانات (مثل `"localhost"`) |
| `user` | String | اسم المستخدم |
| `password` | String | كلمة المرور |
| `db` | String | اسم قاعدة البيانات |
| `port` | Number | رقم المنفذ (عادة `3306`) |

```dz
let db = mysql.Connection("localhost", "root", "pass", "testdb", 3306)
```

---

### `isConnected()` → Bool

التحقق من نجاح الاتصال.

```dz
if db.isConnected():
    print("متصل بقاعدة البيانات")
end
```

---

### `ping()` → Bool

التحقق من أن الخادم ما زال حياً.

```dz
if !db.ping():
    print("فقد الاتصال بالخادم")
end
```

---

### `error()` → String

جلب آخر رسالة خطأ.

```dz
print(db.error())
```

---

### `errno()` → Number

جلب آخر رقم خطأ.

```dz
print(db.errno())
```

---

### `serverInfo()` → String

معلومات إصدار خادم MySQL.

```dz
print(db.serverInfo())
```

---

### `hostInfo()` → String

معلومات عن نوع الاتصال بالخادم.

```dz
print(db.hostInfo())
```

---

### `escape(s)` → String

تأمين نص ضد حقن SQL (إفلات الرموز الخاصة).

```dz
let safe = db.escape(userInput)
```

---

### `query(sql)` → ResultSet | Null

تنفيذ استعلام `SELECT` وإرجاع مجموعة نتائج.

```dz
let res = db.query("SELECT * FROM users")
if res != Null:
    # معالجة النتائج
end
```

---

### `safeQuery(sql, params)` → ResultSet | Null

تنفيذ استعلام آمن باستخدام علامات `?` والبارامترات لمنع حقن SQL.

```dz
let res = db.safeQuery("SELECT * FROM users WHERE age > ? AND city = ?", [18, "Algiers"])
```

---

### `execute(sql)` → Bool

تنفيذ جملة تعديل (`INSERT`, `UPDATE`, `DELETE`, `CREATE TABLE`, إلخ).

```dz
let ok = db.execute("INSERT INTO users (name) VALUES ('Riad')")
```

---

### `safeExecute(sql, params)` → Bool

تنفيذ عملية تعديل آمنة بالبارامترات.

```dz
let ok = db.safeExecute("INSERT INTO users (name, age) VALUES (?, ?)", ["Anis", 25])
```

---

### `affectedRows()` → Number

عدد الصفوف المتأثرة بآخر عملية تعديل.

```dz
let count = db.affectedRows()
```

---

### `insertId()` → Number

المعرف الذي تولّد تلقائياً (AUTO_INCREMENT) من آخر إدراج.

```dz
let newId = db.insertId()
```

---

### `beginTransaction()` → Bool

بدء معاملة (Transaction).

```dz
db.beginTransaction()
```

---

### `commit()` → Bool

تثبيت التغييرات وتخزينها نهائياً.

```dz
db.commit()
```

---

### `rollback()` → Bool

التراجع عن التغييرات والعودة للحالة السابقة.

```dz
db.rollback()
```

---

### `raw()` → Resource

استرجاع الموارد الخام لـ C للاستخدام المباشر مع `__native("_mysql")`.

```dz
let handle = db.raw()
```

---

### `close()`

إغلاق الاتصال وتحرير الموارد.

```dz
db.close()
```

---

## كلاس `ResultSet`

لإدارة نتائج الاستعلامات.

### `numRows()` → Number

عدد الصفوف في النتيجة.

```dz
print(res.numRows())
```

---

### `numFields()` → Number

عدد الأعمدة في النتيجة.

```dz
print(res.numFields())
```

---

### `columns()` → Array

مصفوفة بأسماء الأعمدة.

```dz
let cols = res.columns()
```

---

### `fetchRow()` → Array | Null

جلب الصف التالي كمصفوفة عادية.

```dz
let row = res.fetchRow()
while row != Null:
    print(row[0])
    row = res.fetchRow()
end
```

---

### `fetchAssoc()` → Map | Null

جلب الصف التالي كخريطة (اسم العمود → القيمة).

```dz
let row = res.fetchAssoc()
if row != Null:
    print(row["name"])
end
```

---

### `fetchAll()` → Array

جلب كل الصفوف المتبقية كمصفوفة من المصفوفات.

```dz
let rows = res.fetchAll()
```

---

### `fetchAllAssoc()` → Array

جلب كل الصفوف المتبقية كمصفوفة من الخرائط.

```dz
let users = res.fetchAllAssoc()
for user in users:
    print(user["name"])
end
```

---

### `seek(offset)`

الانتقال إلى صف معين (0-based).

```dz
res.seek(0)  # العودة للبداية
```

---

### `close()`

تحرير موارد النتيجة.

```dz
res.close()
```

---

## مثال تطبيقي متكامل

```dz
use mysql

let db = mysql.Connection("localhost", "root", "", "testdb", 3306)

if db.isConnected():
    db.execute("CREATE TABLE IF NOT EXISTS users (id INT AUTO_INCREMENT PRIMARY KEY, name TEXT, age INT)")

    db.safeExecute("INSERT INTO users (name, age) VALUES (?, ?)", ["Riad", 30])
    db.safeExecute("INSERT INTO users (name, age) VALUES (?, ?)", ["Anis", 25])

    let res = db.safeQuery("SELECT * FROM users WHERE age > ?", [20])
    let users = res.fetchAllAssoc()
    for user in users:
        print("ID: " + str(user["id"]) + ", Name: " + user["name"] + ", Age: " + str(user["age"]))
    end
    res.close()

    db.close()
else:
    print("فشل الاتصال: " + db.error())
end
```
