# Djazair `dweb` Web Framework Manual

**dweb** is a feature-rich, MVC (Model-View-Controller) web framework written entirely in the Djazair programming language. It provides all the essentials for building secure, scalable, and modern web applications and APIs, including dynamic routing, AST-compiled templates, cookie-based sessions, automated CSRF protection, file upload handlers, validation libraries, and a fluent SQL QueryBuilder.

---

## 📋 Table of Contents
1. [Project Directory & Setup](#1-project-directory--setup)
2. [Routing & Controllers](#2-routing--controllers)
3. [Input Validation (Validator API)](#3-input-validation-validator-api)
4. [AST Template Engine](#4-ast-template-engine)
5. [Database & QueryBuilder](#5-database--querybuilder)
6. [Sessions & CSRF Protection](#6-sessions--csrf-protection)
7. [Multipart File Uploads](#7-multipart-file-uploads)
8. [Complete Web MVC Application](#8-complete-web-mvc-application)

---

## 1. Project Directory & Setup

To structure a clean MVC application, we recommend the following folder tree:
```text
my-web-app/
├── app.dz                # Core entry point script
├── controllers/          # Business logic controllers
│   └── UserController.dz
├── models/               # Database models
├── views/                # HTML templates
│   ├── layouts/
│   │   └── base.html     # Base HTML layout
│   ├── index.html        # Main home view
│   └── register.html     # User registration view
└── public/               # Exposed static files folder
    ├── css/
    ├── js/
    └── uploads/          # Folder for user uploads
```

### Installation via DPM:
```bash
dpm install dweb
```
*DPM will automatically fetch and configure database drivers and curl dependency libraries.*

---

## 2. Routing & Controllers

### Main Application Class `dweb.App`
Controls HTTP server lifecycle, middleware registration, and routes mapping.

*   `init(options)`: Instantiates a new application.
    *   *Options:* `"views"` (base directory path for templates. Defaults to script running directory).
*   `setViews(directory)`: Updates views location dynamically.
*   `middleware(handler)`: Registers a global middleware function `fn(req, res, next)`.
*   `serveStatic(url_prefix, directory_path)`: Serve assets folder files (e.g. `app.serveStatic("/assets", "./public")`).
*   `run(port = 8080)`: Fires up the HTTP server listening on specified port.

### Routing Methods:
All routing methods accept `path` (url pattern), `handler` (callback or controller class methods), and an optional array of path-specific `middlewares`.
*   `get(path, handler, middlewares = [])`
*   `post(path, handler, middlewares = [])`
*   `put(path, handler, middlewares = [])`
*   `patch(path, handler, middlewares = [])`
*   `delete(path, handler, middlewares = [])`
*   `group(prefix, callback)`: Encapsulates routing group sharing a prefix path.

### Request Class (The `req` object)
Contains metadata of the incoming request:
*   `req.method`: Request method (GET, POST, etc.).
*   `req.path`: The requested path.
*   `req.headers`: Map containing request headers.
*   `req.cookies`: Map containing client-sent cookies.
*   `req.query`: Map containing URL query parameters.
*   `req.body`: Map containing post form-data or JSON payload.
*   `req.pathParams`: Map containing matched path placeholders (e.g., for `"/users/:id"`, `req.pathParams["id"]` holds the ID).
*   `req.session`: Session store object (if sessions middleware is enabled).

### Response Class (The `res` object)
Controls response generation:
*   `res.status(code)`: Sets HTTP status code (returns response instance for chaining).
*   `res.header(name, value)`: Sets a response header.
*   `res.cookie(name, value, options = {})`: Sends a cookie.
*   `res.send(body)`: Sends plain text response.
*   `res.html(htmlString)`: Sends HTML response with `Content-Type: text/html`.
*   `res.json(data)`: Sends JSON response with `Content-Type: application/json`.
*   `res.redirect(url)`: Triggers client redirection.
*   `res.view(templatePath, data = {})`: Renders template file merging variables.

---

## 3. Input Validation (Validator API)

The `dweb.Validator` provides a fluent validation interface:

```djazair
let validator = dweb.Validator(req.body)
    .required("username").minLength("username", 4)
    .required("email").email("email")
    .optional("website").url("website")
    .required("role").oneOf("role", ["admin", "member"])
```

### Supported Validation Rules:
*   `required(field)`: Field must be present and not empty.
*   `optional(field)`: Skip subsequent checks if field is absent.
*   `string(field)` / `number(field)` / `boolean(field)`: Type validation.
*   `minLength(field, length)` / `maxLength(field, length)`: Checks string length boundaries.
*   `min(field, value)` / `max(field, value)`: Numeric minimum and maximum thresholds.
*   `email(field)`: Checks email pattern match.
*   `url(field)`: Checks URL format match (must begin with `http://` or `https://`).
*   `oneOf(field, arrayChoices)`: Must match one of the array options.
*   `notOneOf(field, arrayChoices)`: Must not match any of the blacklisted options.
*   `same(field, otherField)`: Value must match other field's value (e.g., password confirmation).
*   `different(field, otherField)`: Value must differ from other field.
*   `custom(field, rule_fn)`: Executes user callback `rule_fn(val)`. Must return `True` or error message.

### Validator Results Methods:
*   `passes()`: Returns `True` if inputs contain no errors.
*   `fails()`: Returns `True` if any input violations are found.
*   `errors()`: Returns error map: `{ "field": ["error_msg"] }`.
*   `firstError(field)`: Returns the first validation error string for specified field.
*   `validated()`: Returns a map of successfully validated keys only (prevents unwanted inputs injection).

---

## 4. AST Template Engine

The template engine parses HTML files, compile them into an Abstract Syntax Tree (AST), and renders them with variables.

### Template Engine Features:

#### A. Layout Inheritance & Partials:
Structure templates efficiently using base layout sheets.
In layout base file `views/layouts/base.html`:
```html
<!DOCTYPE html>
<html>
<head>
    <title>{% block title %}Default Title{% endblock %}</title>
</head>
<body>
    {% include "partials/nav.html" %}
    
    <main>
        {% block content %}{% endblock %}
    </main>
</body>
</html>
```
In the child template page `views/index.html`:
```html
{% extends "layouts/base.html" %}

{% block title %}My Home Page{% endblock %}

{% block content %}
    <h1>Welcome Home!</h1>
    <p>This content is injected dynamically.</p>
{% endblock %}
```

#### B. Conditionals:
```html
{% if user.role == "admin" %}
    <p>Welcome Admin!</p>
{% elif user.role == "member" %}
    <p>Welcome Member!</p>
{% else %}
    <p>Welcome Guest!</p>
{% endif %}
```

#### C. Loops:
Loops provide index and element handles in order:
```html
<ul>
    {% for index, u in users %}
        <li>#{{ index + 1 }} - Name: {{ u.name }} - Email: {{ u.email }}</li>
    {% endfor %}
</ul>
```

#### D. Variables Output & XSS Safety:
*   `{{ text }}`: Escapes HTML tags automatically before rendering (XSS protection).
*   `{{{ trusted_html }}}`: Renders raw, unescaped HTML content directly.

---

## 5. Database & QueryBuilder

`dweb` integrates database drivers under a lazy connection design. The database connection opens only on executing the first actual query, avoiding startup lag.

### SQL QueryBuilder
A fluent database query builder with automated parameters escaping to prevent SQL injection.

#### Read Operations:
*   `app.table(db, "name")`: Sets active table.
*   `select(arrayColumns)`: Restrict fields list. (Default: `*`).
*   `where(field, value)` or `where(field, operator, value)`: Adds conditions (e.g. `where("age", ">=", 18)`).
*   `whereIn(field, arrayValues)`: IN clause checking.
*   `orderBy(field, direction = "ASC")`: Sort results.
*   `limit(number)` / `offset(number)`: Range limiting.
*   `get()`: Fetch all matched rows as array of maps.
*   `first()`: Fetch first matched row as map or `Null`.
*   `count()`: Returns matching rows integer count.
*   `exists()`: Check matching rows presence.
*   `paginate(page, perPage)`: Returns map containing results subset, pagination index, and total records count.

#### Write Operations:
*   `insert(mapData)`: Insert a new record. Returns new record's ID.
*   `update(mapData)`: Update record matching `where()` clauses. Returns `True`/`False`.
*   `delete()`: Deletes records matching `where()` clauses. Returns `True`/`False`.

---

## 6. Sessions & CSRF Protection

### Sessions Middleware (`app.sessions(options)`)
Uses encrypted cookies to save session values on the client.
*   *Options:* `"secret"` (cookie encryption passkey), `"lifetime"` (cookie lifespan in seconds).
*   **Methods via `req.session`:**
    *   `set(key, value)` / `get(key, defaultValue)`: Session data storage.
    *   `remove(key)`: Deletes key from session.
    *   `setFlash(key, message)`: Registers a message that persists only for the next request.
    *   `getFlash(key)`: Retrives flash messages.

### Automated CSRF Protection
Enabling the global middleware `app.middleware(app.csrf())` protects mutation routes:
1.  A session token is generated and attached to `req.csrfToken`.
2.  All state-changing requests (POST/PUT/PATCH/DELETE) must include this token in the `_csrf` form field or the `X-CSRF-Token` header.
3.  Invalid tokens reject the request with a `419 - CSRF Token Mismatch` error.

---

## 7. Multipart File Uploads

The `dweb.FileUpload` class handles files incoming from forms with `enctype="multipart/form-data"`:

*   `FileUpload(req, options)`: Sets parser constraints.
    *   *Options:* `"dest"` (target folder), `"maxSize"` (bytes limit), `"allowed"` (extensions whitelist).
*   `upload.file(fieldName)`: Retrieves uploaded file reference.
*   `file.isValid()`: Validates size and extension constraints.
*   `file.save(customName)`: Saves the file. If no custom name is passed, generates a unique timestamped filename.
*   `file.savedPath`: Holds the final file destination path.
*   `file.error`: Error message if file fails constraints check.

---

## 8. Complete Web MVC Application

A complete application managing registered members using SQLite:

```djazair
use dweb
use sqlite

# 1. Initialize Web App View Path and SQLite Database
let app = dweb.App({
    "views": "./views"
})

let db = app.sqlite("portal.db")
db.execute("CREATE TABLE IF NOT EXISTS members (id INTEGER PRIMARY KEY AUTO_INCREMENT, name TEXT, email TEXT, avatar TEXT)")

# 2. Register Global Middlewares
app.middleware(app.sessions({
    "secret": "djazair_portal_secure_passphrase"
}))
app.middleware(app.csrf())

# 3. GET Route for Listing Members
app.get("/", fn(req, res)
    let members = app.table(db, "members").orderBy("id", "DESC").get()
    let successMessage = req.session.getFlash("success")
    let errorsMap = req.session.getFlash("errors")
    
    res.view("index.html", {
        "members": members,
        "success": successMessage,
        "errors": errorsMap,
        "csrf_token": req.csrfToken
    })
end)

# 4. POST Route for Creating Members
app.post("/register", fn(req, res)
    # A. Validate Text Inputs
    let validator = dweb.Validator(req.body)
        .required("name").minLength("name", 3).maxLength("name", 50)
        .required("email").email("email")
        
    if validator.fails()
        req.session.setFlash("errors", validator.errors())
        res.redirect("/")
        return
    end

    # B. Handle File Upload (Avatar Image)
    let upload = dweb.FileUpload(req, {
        "dest": "./public/uploads",
        "maxSize": 3 * 1024 * 1024, # 3MB limit
        "allowed": [".png", ".jpg", ".jpeg"]
    })
    
    let avatarFile = upload.file("avatar")
    let avatarPath = "/uploads/default.png"
    
    if avatarFile.isValid()
        avatarFile.save()
        avatarPath = "/uploads/" + path.basename(avatarFile.savedPath)
    else
        if avatarFile.error != "No file provided for field 'avatar'"
            req.session.setFlash("errors", {"avatar": [avatarFile.error]})
            res.redirect("/")
            return
        end
    end

    # C. Insert to Database
    let validated = validator.validated()
    app.table(db, "members").insert({
        "name": validated["name"],
        "email": validated["email"],
        "avatar": avatarPath
    })

    req.session.setFlash("success", "Member added successfully!")
    res.redirect("/")
end)

# 5. Serve Uploaded Static Files Publicly
app.serveStatic("/uploads", "./public/uploads")

# 6. Run Server
app.run(8080)
```

### Template Layout Source (`views/index.html`):
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
        <!-- CSRF Token -->
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
