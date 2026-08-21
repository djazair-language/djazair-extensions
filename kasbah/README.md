# Kasbah — Web Framework for the Djazair Language

**Version: 0.1.0**  
**Author: Harizi Riyadh (<hariziriyadh@gmail.com>)**

Kasbah is a modern, modular, fast, and secure web framework designed natively for the **Djazair** programming language. Built on top of Djazair's standard `http` module, Kasbah provides an expressive, developer-friendly API inspired by modern global standards (Express, FastAPI, Laravel, Koa).

---

## Key Features

- **Expressive Routing**: Standalone modular routers, route groups, and sub-router mounting (`app.mount("/api", subRouter)`).
- **Core Body Parser**: Automatic parsing for JSON, URL-encoded forms, and multipart uploads without extra configuration.
- **Unified & Intuitive API**: Standard boolean checkers (`isJson()`, `isAjax()`, `isSecure()`, `isType()`) and fluent chained responses.
- **Views & Templating**: Official template engine adapter powered by `Qalam` (similar to Laravel with Blade) via `res.view("name", data)`.
- **Fluent Validation Engine**: Declarative schema validation (`required`, `email`, `number`, `min`, `max`, `oneOf`, `matches`, `custom`).
- **Secure File Uploads**: Automated MIME verification, extension whitelist checking (`hasExt`), and temporary file lifecycle cleanup.
- **Session & Cookie Jars**: Cryptographically signed cookies and persistent file-based sessions.
- **Centralized & Modular Architecture**: Clean separation of concerns with full compliance with Djazair language keywords and syntax.

---

## 1. Installation

```bash
dpm install kasbah
```

Kasbah is a pure-Djazair framework — no native compilation step required.

---

## 2. Quick Start

```djazair
use kasbah

let app = new kasbah.app({
    "port": 3000,
    "static": "public"
})

# Basic HTML route
app.get("/", fn(req, res)
    res.html("<h1>Welcome to Kasbah Web Framework!</h1>")
end)

# Dynamic Route with URL Parameters
app.get("/hello/:name", fn(req, res)
    res.json({
        "message": "Hello, " + req.param("name"),
        "isAjax": req.isAjax()
    })
end)

# Start listening
app.listen()
```

Run your server:
```bash
djazair server.dz
```

---

## 3. Standard Configuration Schema

Kasbah applications are configured via a clean, intuitive configuration dictionary:

```djazair
let app = new kasbah.app({
    # ── Core Options ──────────────────────────────────────────
    "port":        3000,          # TCP port to listen on (default: 3000)
    "host":        "0.0.0.0",     # Binding host address (default: "0.0.0.0")
    "static":      "public",      # Path to static assets folder
    "maxBodySize": 10,            # Maximum request/upload size in MB (default: 10)
    "debug":       False,         # Development error details (default: False)
    "logger":      True,          # Request/error console logger (default: True)
    "trustProxy":  False,         # Trust X-Forwarded-For reverse proxy headers

    # ── Session Management ────────────────────────────────────
    "session": {
        "secret":   "my-secret-key-at-least-32-chars-long", # (Required) >= 32 chars
        "maxAge":   86400,        # Cookie lifetime in seconds (default: 1 day)
        "name":     "kasbah.sid", # Cookie name (default: "kasbah.sid")
        "secure":   False,        # Set Secure flag on HTTPS (default: False)
        "sameSite": "Lax"         # SameSite policy: "Lax", "Strict", "None"
    },

    # ── Advanced Server / HTTP Settings (Optional) ────────────
    "server": {
        "keepAlive":             False,
        "keepAliveTimeout":      30,   # Seconds to wait for next request
        "maxKeepAliveRequests":  100   # Max requests per keep-alive connection
    }
})
```

> **Tip:** You can also enable sessions with a one-liner: `"session": "my-secret-key-at-least-32-chars-long"`.

---

## 4. Routing System

Kasbah provides a robust, standalone routing engine supporting static routes, dynamic parameter extraction, route groups, and modular sub-routers.

### HTTP Methods
```djazair
app.get("/trades", handler)
app.post("/trades", handler)
app.put("/trades/:id", handler)
app.delete("/trades/:id", handler)
app.patch("/trades/:id", handler)
app.head("/trades", handler)
app.options("/trades", handler)
app.all("/any-method", handler)
```

### Route-Level Middleware
Pass single or multiple scoped middlewares directly to any route:
```djazair
# Single route-level middleware
app.get("/dashboard", authGuard, fn(req, res)
    res.ok({"user": req.get("user")})
end)

# Array of route-level middlewares
app.post("/admin/settings", [authGuard, adminOnly], fn(req, res)
    res.ok({"status": "saved"})
end)
```

### Route Groups
Group routes sharing a common URL prefix and middleware pipeline:
```djazair
app.group("/admin", [authMiddleware], fn(g)
    g.get("/dashboard", fn(req, res)
        res.json({"status": "admin dashboard"})
    end)
    
    g.get("/users", [adminOnly], fn(req, res)
        res.json({"users": []})
    end)
end)
```

### Modular Standalone Routers & Mounting
Organize large applications into separate router files and mount them:
```djazair
# routes/api.dz
import "../kasbah/init.dz" as kasbah

let api = new kasbah.router()

api.get("/status", fn(req, res)
    res.ok({"status": "healthy"})
end)

api.get("/users/:id", fn(req, res)
    res.json({"id": req.param("id")})
end)

# app.dz
import "routes/api.dz" as apiRoutes
app.mount("/api/v1", apiRoutes.api)
```

---

## 5. Request (`request`) Object

The `request` wrapper provides standard methods to access client data, headers, and authentication tokens:

### Properties
- `req.method` — HTTP method (e.g. `"GET"`, `"POST"`).
- `req.path` — Normalized, URL-decoded path (e.g. `"/trades/42"`).
- `req.ip` — Client IP address (with reverse proxy support).
- `req.body` — Parsed body dictionary (JSON, form-urlencoded, or multipart fields).
- `req.files` — Map of uploaded file descriptors.
- `req.cookies` — Cookie jar interface for request cookies.
- `req.session` — Session manager instance for reading/writing session data.

### Boolean Checkers (Predicates)
- `req.isJson()` — Returns `True` if client sends or expects JSON (`Accept`, `Content-Type`, or AJAX).
- `req.isAjax()` — Returns `True` if request was sent via XMLHttpRequest.
- `req.isSecure()` — Returns `True` if connection is HTTPS.
- `req.isMethod(m)` — Case-insensitive HTTP method check (`req.isMethod("POST")`).
- `req.isType(category)` — Checks `Content-Type` (`"json"`, `"form"`, `"multipart"`).
- `req.hasFile(fieldName)` — Checks if an uploaded file exists and is saved on disk.
- `req.accepts(mimeType)` — Checks if client Accept header accepts given MIME type.

### Data & Input Getters
- `req.param(name, fallback = Null)` — Extract dynamic route parameter (`:id`).
- `req.query(name = Null, fallback = Null)` — Get URL query parameter or full query map.
- `req.input(name, fallback = Null)` — Automatic lookup across `params` → `query` → `body`.
- `req.all()` — Merges `params`, `query`, and `body` into a unified map.
- `req.only(["title", "price"])` — Filters inputs to only allowed keys.
- `req.except(["_token", "password"])` — Returns all inputs excluding specified keys.

### Headers & Authentication
- `req.header("authorization")` — Get header value (case-insensitive).
- `req.token()` — Automatically extracts Bearer token from `Authorization: Bearer <token>`.
- `req.cookie("theme")` — Get incoming cookie value.
- `req.userAgent()` — Returns client `User-Agent`.
- `req.referer()` — Returns client `Referer` URL.
- `req.host()` — Returns client `Host` header.

### Middleware State Sharing
- `req.set("user", userRecord)` — Store custom attribute in request state.
- `req.get("user")` — Retrieve custom attribute.

---

## 6. Response (`response`) Object

The `response` object provides a fluent, chainable API for building HTTP responses:

### Status & Headers
- `res.status(code)` — Set HTTP status code (`res.status(201)`).
- `res.header(name, value = Null)` — Get header (if 1 arg) or set header (if 2 args).
- `res.removeHeader(name)` — Remove queued response header.
- `res.type("json")` — Set `Content-Type` by extension (`"html"`, `"json"`, `".css"`) or MIME type.
- `res.sendStatus(404)` — Send status code with standard HTTP status text.

### Sending Content
- `res.view(templateName, data = {})` — Renders an HTML template view via Qalam (`views/` directory).
- `res.json(data)` — Sends data serialized as JSON (`application/json; charset=utf-8`).
- `res.html(content)` — Sends HTML string (`text/html; charset=utf-8`).
- `res.text(content)` — Sends plain text string (`text/plain; charset=utf-8`).
- `res.xml(content)` — Sends XML string (`application/xml; charset=utf-8`).
- `res.send(raw)` — Sends raw response body.

### File Streaming & Downloads
- `res.file(filePath)` — Streams file from disk with accurate byte `Content-Length` and MIME type.
- `res.download(filePath, filename = Null)` — Triggers browser download dialog with UTF-8 filename encoding (RFC 5987).

### Redirects & Cookies
- `res.redirect(url, code = 302)` — Sends HTTP redirect.
- `res.back(fallback = "/")` — Redirects client back to their `Referer` URL.
- `res.cookie(name, value, options = {})` — Queues `Set-Cookie` header (`maxAge`, `secure`, `httpOnly`, `sameSite`, `path`).
- `res.clearCookie(name)` — Deletes cookie on client.

### Semantic Response Helpers
- `res.ok(data)` — `200 OK` (JSON or empty).
- `res.created(data)` — `201 Created`.
- `res.noContent()` — `204 No Content`.
- `res.badRequest("Invalid payload")` — `400 Bad Request` JSON.
- `res.unauthorized("Unauthorized access")` — `401 Unauthorized` JSON.
- `res.forbidden("Access forbidden")` — `403 Forbidden` JSON.
- `res.notFound("Resource not found")` — `404 Not Found` JSON.
- `res.validationError(errorsMap)` — `422 Unprocessable Entity` JSON.
- `res.serverError("Internal failure")` — `500 Internal Error` JSON.

---

## 7. Input Validation Engine (`validator`)

Kasbah includes a robust, fluent validation engine:

```djazair
app.post("/trades", fn(req, res)
    let v = req.validate()
    
    v.field("symbol").required().string().minLength(2).maxLength(10)
    v.field("type").required().oneOf(["BUY", "SELL"])
    v.field("quantity").required().number().min(1)
    v.field("price").required().number().min(0.01)
    v.field("email").optional().email()
    v.field("website").optional().url()

    if v.fails()
        return res.validationError(v.errors())
    end

    let validatedData = v.validated()
    # Proceed to save trade...
    res.created(validatedData)
end)
```

---

## 8. File Uploads & FileManager

Kasbah provides two complementary APIs for handling file uploads:

### Method 1 — Simple Upload via `req.file()`

Use `req.file(fieldName)` for straightforward single-file uploads with manual validation:

```djazair
app.post("/trades/upload", fn(req, res)
    if not req.hasFile("screenshot")
        return res.badRequest("No screenshot uploaded")
    end

    let file = req.file("screenshot")

    # Validate: must be an image with allowed extension
    if not file.isImage() or not file.hasExt(["jpg", "jpeg", "png", "webp"])
        return res.badRequest("Invalid image format")
    end

    # Save to destination directory
    let savedPath = file.saveTo("public/uploads")
    res.ok({"url": "/" + savedPath})
end)
```

**`uploadedFile` instance methods:**

| Method | Description |
|---|---|
| `file.isValid()` | Returns `True` if the temp file exists on disk |
| `file.isImage()` | Checks MIME type or extension against common image types |
| `file.hasExt(["png", "jpg"])` | Checks if extension is in the allowed list |
| `file.getFilename()` | Returns the original client filename |
| `file.getMimeType()` | Returns the detected MIME type string |
| `file.getExt()` | Returns the lowercase file extension (without dot) |
| `file.getSize()` | Returns file size in bytes |
| `file.saveTo(dir)` | Moves file to target directory; returns saved path |
| `file.saveTo(dir, "custom.png")` | Saves with a custom filename override |
| `file.readBytes()` | Returns file contents as a byte array |
| `file.readText()` | Returns file contents as a UTF-8 string |
| `file.delete()` | Manually deletes the temporary file |

---

### Method 2 — Advanced Upload via `req.uploader()` (FileManager)

Use `req.uploader()` for controlled uploads with automatic type validation, size limits, and batch processing:

```djazair
app.post("/profile/avatar", fn(req, res)
    let uploader = req.uploader({
        "dest":            "public/avatars",   # Destination directory
        "allowedTypes":    ["image/*"],         # MIME type whitelist (supports wildcards)
        "maxSize":         2,                   # Maximum file size in MB
        "keepOriginalName": False,              # Generate unique name (default)
        "prefix":          "avatar_"           # Prefix for generated filenames
    })

    let result = uploader.single("avatar")
    if not result["ok"]
        return res.badRequest(result["error"])
    end

    let savedFile = result["file"]
    res.ok({
        "url":  "/" + savedFile["path"],
        "name": savedFile["filename"],
        "size": savedFile["size"]
    })
end)
```

**Multiple files upload:**
```djazair
app.post("/gallery", fn(req, res)
    let uploader = req.uploader({
        "dest":         "public/gallery",
        "allowedTypes": ["image/jpeg", "image/png", "image/webp"],
        "maxSize":      5
    })

    # Accept up to 10 files from the "photos" field
    let result = uploader.multiple("photos", 10)
    if not result["ok"]
        return res.badRequest(result["errors"])
    end

    res.ok({"uploaded": result["files"]})
end)
```

**`uploader` options:**

| Option | Type | Default | Description |
|---|---|---|---|
| `dest` | String | `"uploads"` | Destination directory |
| `allowedTypes` | Array | `Null` (all) | Permitted MIME types; supports wildcards like `"image/*"` |
| `maxSize` | Number | `Null` (no limit) | Maximum file size in MB per file |
| `keepOriginalName` | Boolean | `False` | Preserve client filename; if `False`, generates a unique name |
| `prefix` | String | `""` | Prefix prepended to generated filenames |

**`uploader` methods:**

| Method | Description |
|---|---|
| `uploader.single(field)` | Process one file from the given form field |
| `uploader.multiple(field, maxCount)` | Process multiple files (default limit: 10) |
| `uploader.any()` | Process all uploaded files from all fields |
| `uploader.info(path)` | Get metadata for an existing file on disk |
| `uploader.remove(path)` | Delete a file from disk |

---

## 9. Error Handling & 404 Customization

```djazair
# Custom 404 Not Found handler
app.onNotFound(fn(req, res)
    if req.isJson()
        res.notFound("Endpoint does not exist")
    else
        res.status(404).html("<h1>404 Not Found</h1><p>Path: " + req.path + "</p>")
    end
end)

# Custom 500 Error handler
app.onError(fn(err, req, res)
    res.status(500).html("<h1>500 Internal Server Error</h1><p>" + str(err) + "</p>")
end)
```

---

## 10. Complete Application Example

```djazair
use kasbah

let app = new kasbah.app({
    "port": 3000,
    "static": "public",
    "session": "secret-encryption-key-at-least-32-chars-long"
})

# Global Middleware
app.middleware(fn(req, res)
    print("[LOG] " + req.method + " " + req.path)
end)

# HTML Route
app.get("/", fn(req, res)
    res.html("<h1>Welcome to Kasbah</h1><p>Full-featured web framework for Djazair</p>")
end)

# REST API with Validation
app.post("/api/trades", fn(req, res)
    let v = req.validate()
    v.field("symbol").required().string()
    v.field("price").required().number().min(0)

    if v.fails()
        return res.validationError(v.errors())
    end

    res.created({"message": "Trade created successfully", "trade": v.validated()})
end)

# Start Application
app.listen()
```

---

## 11. Views & Templating (Powered by Qalam)

Similar to how **Laravel** integrates **Blade**, Kasbah provides an official View adapter powered by **Qalam** out of the box. You can configure your template directory in the application options and render views directly via `res.view()`:

```djazair
use kasbah

let app = new kasbah.app({
    "port":  3000,
    "views": "./views"
})

# Renders views/trades/list.html with data context
app.get("/trades", fn(req, res)
    res.view("trades/list", {
        "title":  "My Trades",
        "trades": tradeList
    })
end)

app.listen()
```

### Template Syntax Overview (Qalam)
- **Variables (Escaped)**: `{{ user.name }}` (automatic XSS prevention)
- **Raw HTML**: `{{{ raw_html_content }}}`
- **Conditionals**: `{% if status == 'Active' %} ... {% else %} ... {% endif %}`
- **Loops**: `{% for item in items %} ... {% endfor %}`
- **Layout Inheritance**: `{% extends "layouts/main" %}` and `{% block content %} ... {% endblock %}`
- **Partials / Subviews**: `{% include "partials/nav" %}`
- **Standalone View Engine**: `let v = new kasbah.view({"views": "./views"})`

---

## License

Kasbah is open-source software licensed under the **MIT License**.
