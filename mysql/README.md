# Djazair MySQL Extension Manual

The **MySQL** extension provides database connectivity to MySQL and MariaDB servers from the Djazair programming language. It supports parameterized queries to prevent SQL injection, transaction management, and multiple result set fetch modes.

---

## Table of Contents
1. [Installation](#1-installation)
2. [Quick Start](#2-quick-start)
3. [Connection Class API](#3-connection-class-api)
4. [ResultSet Class API](#4-resultset-class-api)
5. [Transactions](#5-transactions)
6. [Error Handling](#6-error-handling)
7. [Complete Examples](#7-complete-examples)

---

## 1. Installation

Make sure the MySQL extension is built:

```bash
cd extensions/mysql
.\build.bat
```

Then import it in your script:

```djazair
use mysql
```

---

## 2. Quick Start

```dz
use mysql

let db = mysql.Connection("localhost", "root", "", "testdb", 3306)

if db.isConnected()
    print("Connected to " + db.serverInfo())

    db.execute("CREATE TABLE IF NOT EXISTS users (id INT AUTO_INCREMENT PRIMARY KEY, name TEXT)")
    db.safeExecute("INSERT INTO users (name) VALUES (?)", ["Riad"])

    let res = db.query("SELECT * FROM users")
    if !isNull(res)
        let rows = res.fetchAllAssoc()
        for row in rows
            print(row["name"])
        end
        res.close()
    end

    db.close()
else
    print("Connection failed: " + db.error())
end
```

---

## 3. Connection Class API

Manages the database connection session and query executions.

### `init(host, user, password, db, port = 3306)`
Opens a new MySQL connection with `utf8mb4` charset.

```dz
let db = mysql.Connection("localhost", "root", "password", "testdb", 3306)
let db2 = mysql.Connection("192.168.1.10", "app", "secret", "mydb", 3307)
```

### `isConnected()` → Bool
Returns `True` if the connection is active.

```dz
if db.isConnected()
    print("Connected!")
end
```

### `ping()` → Bool
Pings the server to check connection health.

```dz
if !db.ping()
    print("Connection lost, reconnecting...")
end
```

### `error()` → String
Returns the last error message.

```dz
print(db.error())
```

### `errno()` → Number
Returns the last MySQL error code.

```dz
print(db.errno())
```

### `serverInfo()` → String
Returns the MySQL server version string.

```dz
print(db.serverInfo())
```

### `hostInfo()` → String
Returns connection type description.

```dz
print(db.hostInfo())
```

### `escape(s)` → String
Escapes special characters in a string for manual SQL building.

```dz
let safe = db.escape(userInput)
```

### `query(sql)` → ResultSet | Null
Executes a raw SELECT query and returns a ResultSet.

```dz
let res = db.query("SELECT * FROM users")
```

### `safeQuery(sql, params)` → ResultSet | Null
Executes a parameterized SELECT query with `?` placeholders.

```dz
let res = db.safeQuery("SELECT * FROM users WHERE age > ? AND city = ?", [18, "Algiers"])
```

### `execute(sql)` → Bool
Executes a non-SELECT statement (INSERT, UPDATE, DELETE, DDL).

```dz
let ok = db.execute("CREATE TABLE test (id INT)")
```

### `safeExecute(sql, params)` → Bool
Executes a parameterized non-SELECT statement.

```dz
let ok = db.safeExecute("INSERT INTO users (name, age) VALUES (?, ?)", ["Anis", 25])
```

### `affectedRows()` → Number
Rows affected by the last INSERT, UPDATE, or DELETE.

```dz
db.safeExecute("UPDATE users SET age = ? WHERE id = ?", [26, 1])
print("Updated " + str(db.affectedRows()) + " rows")
```

### `insertId()` → Number
The AUTO_INCREMENT ID from the last INSERT.

```dz
db.safeExecute("INSERT INTO users (name) VALUES (?)", ["NewUser"])
print("New ID: " + str(db.insertId()))
```

### `beginTransaction()` → Bool
Starts a new transaction.

```dz
db.beginTransaction()
```

### `commit()` → Bool
Commits the current transaction.

```dz
db.commit()
```

### `rollback()` → Bool
Rolls back the current transaction.

```dz
db.rollback()
```

### `raw()` → Resource
Returns the underlying C handle for direct `_mysql` API calls.

```dz
let handle = db.raw()
```

### `close()`
Closes the connection and frees resources.

```dz
db.close()
```

---

## 4. ResultSet Class API

Manages records returned by `query()` or `safeQuery()`.

### `numRows()` → Number
Number of rows in the result set.

```dz
print("Rows: " + str(res.numRows()))
```

### `numFields()` → Number
Number of columns per row.

```dz
print("Columns: " + str(res.numFields()))
```

### `columns()` → Array
Array of column name strings.

```dz
print(str(res.columns()))
```

### `fetchRow()` → Array | Null
Next row as a zero-indexed array.

```dz
let row = res.fetchRow()
while !isNull(row)
    print(row[0])
    row = res.fetchRow()
end
```

### `fetchAssoc()` → Map | Null
Next row as an associative map keyed by column name.

```dz
let user = res.fetchAssoc()
if !isNull(user)
    print(user["name"])
end
```

### `fetchAll()` → Array
All remaining rows as an array of arrays.

```dz
let rows = res.fetchAll()
for r in rows
    print(str(r))
end
```

### `fetchAllAssoc()` → Array
All remaining rows as an array of maps.

```dz
let rows = res.fetchAllAssoc()
for r in rows
    print(r["name"])
end
```

### `seek(offset)`
Moves the internal row pointer (0-based).

```dz
res.seek(0)  # reset to first row
```

### `close()`
Frees the result set memory.

```dz
res.close()
```

---

## 5. Transactions

Use `beginTransaction()`, `commit()`, and `rollback()` to group statements.

```dz
use mysql

let db = mysql.Connection("localhost", "root", "", "testdb", 3306)

if db.isConnected()
    if db.beginTransaction()
        db.safeExecute("INSERT INTO accounts (owner, balance) VALUES (?, ?)", ["Riad", 1000.0])
        db.safeExecute("INSERT INTO accounts (owner, balance) VALUES (?, ?)", ["Anis", 500.0])

        if db.commit()
            print("Transaction committed")
        else
            print("Commit failed, rolling back: " + db.error())
            db.rollback()
        end
    else
        print("Could not start transaction: " + db.error())
    end

    db.close()
else
    print("Connection failed: " + db.error())
end
```

---

## 6. Error Handling

Always check connection status and query results.

```dz
use mysql

let db = mysql.Connection("localhost", "root", "", "testdb", 3306)

if db.isConnected()
    # Bad SQL triggers error
    if !db.execute("INVALID SQL")
        print("Error: " + db.error())
        print("Code: " + str(db.errno()))
    end

    # safeQuery returns Null on failure
    let res = db.safeQuery("SELECT * FROM nonexistent")
    if isNull(res)
        print("Query failed: " + db.error())
    end

    # ping to verify connection health
    if db.ping()
        print("Server alive")
    end

    db.close()

    # Methods return safe defaults on closed connection
    if !db.isConnected()
        print("Closed — ping: " + str(db.ping()))
    end
end
```

---

## 7. Complete Examples

### 7.1. Basic CRUD

```dz
use mysql

let db = mysql.Connection("localhost", "root", "", "testdb", 3306)

if db.isConnected()
    db.execute("CREATE TABLE IF NOT EXISTS products (id INT AUTO_INCREMENT PRIMARY KEY, name TEXT, price REAL)")

    db.safeExecute("INSERT INTO products (name, price) VALUES (?, ?)", ["Laptop", 1200.0])
    db.safeExecute("INSERT INTO products (name, price) VALUES (?, ?)", ["Mouse", 25.0])
    db.safeExecute("INSERT INTO products (name, price) VALUES (?, ?)", ["Keyboard", 75.0])

    let res = db.safeQuery("SELECT * FROM products WHERE price > ?", [50.0])
    if !isNull(res)
        let items = res.fetchAllAssoc()
        for item in items
            print(item["name"] + " — $" + str(item["price"]))
        end
        res.close()
    end

    db.close()
end
```

### 7.2. Parameterized Queries

```dz
use mysql

let db = mysql.Connection("localhost", "root", "", "testdb", 3306)

if db.isConnected()
    db.execute("CREATE TABLE IF NOT EXISTS employees (id INT AUTO_INCREMENT PRIMARY KEY, name TEXT, dept TEXT)")

    # Insert with safeExecute
    db.safeExecute("INSERT INTO employees (name, dept) VALUES (?, ?)", ["Alice", "Engineering"])
    db.safeExecute("INSERT INTO employees (name, dept) VALUES (?, ?)", ["Bob", "Marketing"])

    # Query with LIKE and placeholder
    let res = db.safeQuery("SELECT * FROM employees WHERE dept LIKE ?", ["%Eng%"])
    if !isNull(res)
        let rows = res.fetchRow()
        while !isNull(rows)
            print(rows[1] + " — " + rows[2])
            rows = res.fetchRow()
        end
        res.close()
    end

    db.close()
end
```

### 7.3. Fetch Variants

```dz
use mysql

let db = mysql.Connection("localhost", "root", "", "testdb", 3306)

if db.isConnected()
    db.execute("CREATE TABLE IF NOT EXISTS items (id INT AUTO_INCREMENT PRIMARY KEY, label TEXT)")
    db.safeExecute("INSERT INTO items (label) VALUES (?) ", ["Alpha"])
    db.safeExecute("INSERT INTO items (label) VALUES (?) ", ["Beta"])
    db.safeExecute("INSERT INTO items (label) VALUES (?) ", ["Gamma"])

    let res = db.query("SELECT * FROM items ORDER BY id")

    if !isNull(res)
        print("Columns: " + str(res.columns()))

        # fetchRow
        res.seek(0)
        let r = res.fetchRow()
        while !isNull(r)
            print("Row: " + str(r))
            r = res.fetchRow()
        end

        # fetchAllAssoc
        res.seek(0)
        let all = res.fetchAllAssoc()
        for a in all
            print(a["label"])
        end

        res.close()
    end

    db.close()
end
```

All examples are available in the `examples/` directory.
