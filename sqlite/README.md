# Djazair SQLite Extension Manual

The **SQLite** extension provides a lightweight, local database engine for the Djazair programming language. It supports synchronous operations, transaction controls, and query parameters escaping to prevent SQL injection.

---

## Table of Contents
1. [Importing](#1-importing)
2. [Connection Class](#2-connection-class)
3. [ResultSet Class](#3-resultset-class)
4. [Constants](#4-constants)
5. [Complete Example](#5-complete-example)

---

## 1. Importing

To import the SQLite module, use:
```djazair
use sqlite
```

---

## 2. Connection Class

Handles database files operations and query execution.

### Methods

| Method | Description |
| :--- | :--- |
| `sqlite.connect(path)` | Opens a database file. Creates the file automatically if it does not exist. |
| `isConnected()` | Returns `True` if the connection is active and valid. |
| `query(sql)` | Executes a query and returns a `ResultSet` object. Typically used for SELECT statements. |
| `execute(sql)` | Executes DDL/DML statements (CREATE, INSERT, UPDATE, DELETE). Returns `True`/`False`. |
| `safeQuery(sql, params)` | Executes a query safely replacing `?` placeholders with escaped params. |
| `safeExecute(sql, params)` | Executes DDL/DML statements safely replacing `?` placeholders with escaped params. |
| `changes()` | Returns the number of database rows modified by the last query. |
| `insertId()` | Returns the last generated AUTOINCREMENT row ID. |
| `beginTransaction()` | Begins a database transaction. |
| `commit()` | Commits the active transaction. |
| `rollback()` | Rolls back the active transaction. |
| `escape(val)` | Escapes special characters manually. |
| `error()` | Retrieves the last error message string returned by SQLite. |
| `raw()` | Returns the native C pointer resource of the database handle. |
| `close()` | Closes connection and frees active handle. |

---

## 3. ResultSet Class

Retrieves and iterates over rows returned by queries.

### Methods

| Method | Description |
| :--- | :--- |
| `fetchRow()` | Fetches the next row as an indexed array. Returns `Null` if no more records exist. |
| `fetchAssoc()` | Fetches the next row as a map (`{ "column": value }`). Returns `Null` if no more records exist. |
| `fetchAllAssoc()` | Fetches all remaining records as an array of maps. |
| `columns()` | Returns an array of column names in the result set. |
| `raw()` | Returns the native C pointer resource of the active statement handle. |
| `close()` | Finalizes the statement and frees memory. |

---

## 4. Constants

| Constant | Value | Description |
|---|---|---|
| `sqlite.SQLITE_ROW` | `100` | Indicates the statement step found a new row ready for retrieval. |
| `sqlite.SQLITE_DONE` | `101` | Indicates the statement step completed execution successfully (no more rows). |

> See the [`examples/`](examples/) directory for runnable demos covering CRUD, transactions, rollback, fetch modes, and error handling.

---

## 5. Complete Example

```djazair
use sqlite

# 1. Open database connection
let db = sqlite.connect("school.db")

if db.isConnected()
    # 2. Create tables
    db.execute("CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY, name TEXT, score REAL)")
    
    # 3. Insert records safely
    db.safeExecute("INSERT INTO users (name, score) VALUES (?, ?)", ["Riad", 98.5])
    db.safeExecute("INSERT INTO users (name, score) VALUES (?, ?)", ["Anis", 89.0])
    
    # 4. Query records safely
    let res = db.safeQuery("SELECT * FROM users WHERE score > ?", [90.0])
    if !isNull(res)
        # Display column headers
        print("Columns: " + str(res.columns()))
        
        # Read records as maps
        let users = res.fetchAllAssoc()
        let i = 0
        while i < users.length()
            let user = users[i]
            print("User [ID: " + str(user["id"]) + ", Name: " + user["name"] + ", Score: " + str(user["score"]) + "]")
            i += 1
        end
        
        res.close()
    end
    
    db.close()
    print("Process finished successfully.")
else
    print("Failed to connect to database: " + db.error())
end
```
