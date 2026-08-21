# Kasbah — Full-Featured Web Framework for Djazair

<p align="center">
  <strong>Fast, Modular, Expressive & Secure Web Framework for the Djazair Programming Language</strong>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/version-0.2.0-blue.svg?style=flat-square" alt="Version">
  <img src="https://img.shields.io/badge/language-Djazair-green.svg?style=flat-square" alt="Language">
  <img src="https://img.shields.io/badge/type-Pure%20Djazair-purple.svg?style=flat-square" alt="Pure Djazair">
  <img src="https://img.shields.io/badge/license-MIT-orange.svg?style=flat-square" alt="License">
</p>

---

## Overview

**Kasbah** is a production-ready, full-stack web framework designed natively for the **Djazair** programming language. Built upon Djazair's standard `http` engine, Kasbah brings the architectural elegance, expressive ergonomics, and robustness of modern global standards (Express, Fastify, Laravel, FastAPI, Spring) into the Djazair ecosystem.

Whether building high-performance RESTful microservices, secure JSON APIs, or dynamic server-rendered web applications with the `Qalam` template engine, Kasbah provides a unified and cohesive developer experience.

---

## Key Highlights

- 🚀 **Zero Native Dependencies**: 100% Pure Djazair codebase — seamless portability across Windows, Linux, and macOS.
- ⚡ **High-Performance Routing**: Ultra-fast static routing, parameterized dynamic segments (`:param`), route groups, and mountable sub-routers.
- 🛡️ **Enterprise-Grade Security**: Native **CORS** middleware, **Helmet** security headers, strict cookie signing (HMAC-SHA256), CRLF protection, and path traversal guards.
- 📦 **Automated Body Parsing**: Multi-format parsing for JSON, URL-encoded forms, and multipart file uploads out of the box.
- 🧩 **Modular Middleware Pipeline**: Onion-model request/response interceptors, route-level middleware, and scoped group pipelines.
- 🖋️ **Qalam Template Integration**: Official View adapter enabling full MVC server-side rendering with template inheritance (`res.view()`).
- ✨ **Fluent Validation DSL**: Declarative input sanitization and validation schema with rich constraint rules.
- 📂 **Streaming File Uploads & Downloads**: Robust file manager with automatic temp cleanup, MIME type validation, and RFC 5987 Unicode filename support.

---

## Table of Contents

1. [Installation](#1-installation)
2. [Quick Start](#2-quick-start)
3. [Architecture & Request Lifecycle](#3-architecture--request-lifecycle)
4. [Application Configuration](#4-application-configuration)
5. [Routing & Sub-Routers](#5-routing--sub-routers)
6. [Request Interface (`req`)](#6-request-interface-req)
7. [Response Interface (`res`)](#7-response-interface-res)
8. [Built-in Security Middleware](#8-built-in-security-middleware)
   - [CORS Middleware (`kasbah.cors`)](#cors-middleware-kasbahcors)
   - [Helmet Security Headers (`kasbah.helmet`)](#helmet-security-headers-kasbahhelmet)
   - [Static File Serving (`kasbah.serveStatic`)](#static-file-serving-kasbahservestatic)
   - [Structured Logging (`kasbah.logger`)](#structured-logging-kasbahlogger)
9. [Session & Cookie Management](#9-session--cookie-management)
10. [Input Validation Engine (`kasbah.validator`)](#10-input-validation-engine-kasbahvalidator)
11. [File Uploads & File Manager](#11-file-uploads--file-manager)
12. [Template Engine Integration (`Qalam`)](#12-template-engine-integration-qalam)
13. [Centralized Error Handling](#13-centralized-error-handling)
14. [Full REST Application Example](#14-full-rest-application-example)
15. [Running Tests](#15-running-tests)
16. [License](#16-license)

---

## 1. Installation

Install via the Djazair Package Manager (`dpm`):

```bash
dpm install kasbah
```

Or clone directly into your project's extension folder:

```bash
git clone https://github.com/djazair-language/djazair-extensions.git
```

---

## 2. Quick Start

Create an entry file `server.dz`:

```djazair
use kasbah

# Initialize application
let app = new kasbah.app({
    "port":   8080,
    "static": "public"
})

# Basic JSON endpoint
app.get("/", fn(req, res)
    res.json({
        "framework": "Kasbah",
        "version":   "0.1.0",
        "status":    "running"
    })
end)

# Dynamic Route Parameter
app.get("/hello/:name", fn(req, res)
    res.ok({
        "greeting": "Hello, " + req.param("name") + "!",
        "isAjax":   req.isAjax()
    })
end)

# Start web server
app.listen()
```

Run with the Djazair runtime:

```bash
djazair server.dz
```

---

## 3. Architecture & Request Lifecycle

Kasbah executes incoming HTTP requests through a structured, linear pipeline:

```text
[ Incoming HTTP Request ]
          │
          ▼
┌────────────────────────────────────────┐
│ 1. Request Preparation                 │
│    • IP Resolution (Proxy / Remote)    │
│    • Path Decoding & Traversal Guard   │
│    • CookieJar Initialization          │
└────────────────────────────────────────┘
          │
          ▼
┌────────────────────────────────────────┐
│ 2. Core Middleware Pipeline            │
│    • BodyParser (JSON / Form / Multi)  │
│    • Session Initialization            │
│    • ServeStatic Assets Handler        │
│    • User Middleware (CORS / Helmet)   │
└────────────────────────────────────────┘
          │
          ▼
┌────────────────────────────────────────┐
│ 3. Router & Route Middlewares          │
│    • Path & Verb Matching              │
│    • Parameter Extraction (:param)     │
│    • Scoped Route Middlewares          │
│    • Route Handler Invocation          │
└────────────────────────────────────────┘
          │
          ▼
┌────────────────────────────────────────┐
│ 4. Response Dispatch & Teardown        │
│    • Byte-accurate Content-Length      │
│    • Cookie Injection (RFC 6265)       │
│    • Session Writeback & Persistence   │
│    • Temporary Upload Files Cleanup    │
│    • Access Log Latency Reporting      │
└────────────────────────────────────────┘
          │
          ▼
[ HTTP Client Response ]
```

---

## 4. Application Configuration

Kasbah applications accept a comprehensive configuration map upon instantiation:

```djazair
let app = new kasbah.app({
    # ── Network & Server Binding ──────────────────────────────────────────────
    "port":        8080,          # TCP Port (default: 3000)
    "host":        "0.0.0.0",     # Binding Address (default: "0.0.0.0")
    "static":      "public",      # Path to static directory
    "views":       "views",       # Path to Qalam templates directory
    "maxBodySize": 10,            # Max request body size in MB (default: 10)
    "trustProxy":  False,         # Trust X-Forwarded-For reverse proxy headers
    "debug":       False,         # Detailed error outputs in response

    # ── Logging ───────────────────────────────────────────────────────────────
    "logger":      True,          # Boolean or config map: {"level": "info", "format": "dev"}

    # ── Session Security ──────────────────────────────────────────────────────
    "session": {
        "secret":   "your-cryptographic-secret-at-least-32-chars-long!", # Required (>= 32 chars)
        "name":     "kasbah.sid", # Cookie name
        "maxAge":   86400,        # Expiry in seconds (1 day)
        "secure":   False,        # HTTPS only flag
        "sameSite": "Lax",        # SameSite: "Lax", "Strict", "None"
        "dir":      ""            # Optional custom session storage directory
    },

    # ── Keep-Alive Settings (Optional) ────────────────────────────────────────
    "server": {
        "keepAlive":             False,
        "keepAliveTimeout":      30,
        "maxKeepAliveRequests":  100
    }
})
```

---

## 5. Routing & Sub-Routers

### Standard HTTP Methods

```djazair
app.get("/items", handler)
app.post("/items", handler)
app.put("/items/:id", handler)
app.delete("/items/:id", handler)
app.patch("/items/:id", handler)
app.head("/items", handler)
app.options("/items", handler)
app.all("/any-method", handler)
```

### Dynamic URL Parameters

Parameters prefixed with `:` are automatically extracted into `req.params` and accessible via `req.param(name)`:

```djazair
app.get("/users/:userId/orders/:orderId", fn(req, res)
    let uid = req.param("userId")
    let oid = req.param("orderId")
    res.ok({"userId": uid, "orderId": oid})
end)
```

### Route-Level Middleware

Attach single or multiple scoped middlewares directly to specific routes:

```djazair
# Single route middleware
app.get("/dashboard", authGuard, fn(req, res)
    res.ok({"user": req.get("user")})
end)

# Array of route middlewares
app.post("/admin/settings", [authGuard, adminOnly], fn(req, res)
    res.ok({"status": "updated"})
end)
```

### Route Groups

Organize routes sharing common prefixes and middleware pipelines:

```djazair
app.group("/api/v1", [authMiddleware], fn(g)
    g.get("/profile", fn(req, res)
        res.json({"profile": "data"})
    end)

    g.group("/admin", [adminOnly], fn(adminGroup)
        adminGroup.get("/stats", fn(req, res)
            res.json({"stats": []})
        end)
    end)
end)
```

### Standalone Sub-Routers & Mounting

Modularize large codebases into standalone routers and mount them on prefixes:

```djazair
# routes/trades.dz
import "kasbah/init.dz" as kasbah

let router = new kasbah.router()

router.get("/", fn(req, res)
    res.ok({"trades": []})
end)

router.get("/:id", fn(req, res)
    res.ok({"tradeId": req.param("id")})
end)

# server.dz
import "routes/trades.dz" as tradesRoutes
app.mount("/api/trades", tradesRoutes.router)
```

---

## 6. Request Interface (`req`)

The `request` object wraps incoming client data with safe, fluent accessors:

### Properties
| Property | Type | Description |
|:---|:---|:---|
| `req.method` | `String` | HTTP verb (`"GET"`, `"POST"`, `"PUT"`, etc.) |
| `req.path` | `String` | URL path without query string (`"/trades/42"`) |
| `req.ip` | `String` | Client IP address (considers `trustProxy`) |
| `req.body` | `Map\|String` | Parsed request payload (JSON / Form map) |
| `req.files` | `Map` | Uploaded multipart files descriptor dictionary |
| `req.cookies` | `Object` | CookieJar instance for request cookies |
| `req.session` | `Object` | SessionManager instance for active session |

### Input Extraction Methods
```djazair
# 1. Individual getters
let id   = req.param("id", "default_id") # Route param (:id)
let page = req.query("page", "1")        # Query string (?page=1)
let data = req.input("email")            # Looks up param -> query -> body in order

# 2. Bulk input dictionaries
let allInputs = req.all()                # Merges params + query + body
let allInputs = req.inputs()             # Alias for req.all()
let filtered  = req.only(["name", "age"])# Returns map with only allowed keys
let omitted   = req.except(["password"]) # Returns map excluding omitted keys
```

### Inspection Predicates
```djazair
req.isJson()       # True if client expects or sends JSON
req.isAjax()       # True if X-Requested-With: XMLHttpRequest
req.isSecure()     # True if connection is HTTPS
req.isMethod("GET")# Case-insensitive HTTP verb match
req.isType("json") # Matches Content-Type ("json", "form", "multipart")
req.hasFile("doc") # True if uploaded file exists on disk
req.accepts("html")# True if client accepts requested MIME type
```

### Headers, Auth & State
```djazair
let auth    = req.header("authorization") # Case-insensitive header lookup
let token   = req.token()                 # Extracts Bearer token
let ua      = req.userAgent()             # Client User-Agent string
let referer = req.referer()               # Client Referer / Referrer URL
let host    = req.host()                  # Client Host header

# Middleware state sharing
req.set("currentUser", userObject)
let user = req.get("currentUser")
```

---

## 7. Response Interface (`res`)

The `response` object provides a fluent, chainable API for crafting HTTP responses:

### Content Senders
```djazair
res.json({"status": "ok"})          # application/json; charset=utf-8
res.html("<h1>Hello</h1>")          # text/html; charset=utf-8
res.text("Plain text")              # text/plain; charset=utf-8
res.xml("<root></root>")            # application/xml; charset=utf-8
res.view("home", {"name": "Riyadh"})# Renders Qalam template views/home.html
res.send(rawContent)                # Raw string body
```

### File Streaming & Downloads
```djazair
# Streams binary/text file with accurate MIME type & Content-Length
res.file("assets/report.pdf")

# Triggers browser download dialog with RFC 5987 Unicode filename
res.download("assets/document.pdf", "تقرير_السنوي.pdf")
```

### Semantic Status Helpers
```djazair
res.ok(data)               # 200 OK (JSON or empty)
res.created(data)          # 201 Created (JSON or empty)
res.noContent()            # 204 No Content
res.badRequest("Invalid")  # 400 Bad Request
res.unauthorized("Auth")   # 401 Unauthorized
res.forbidden("Forbidden") # 403 Forbidden
res.notFound("Not Found")  # 404 Not Found
res.validationError(errors)# 422 Unprocessable Entity
res.serverError("Error")   # 500 Internal Server Error
```

### Headers & Cookies
```djazair
res.status(201)
res.header("X-Custom-Header", "Value")
res.removeHeader("X-Powered-By")
res.type("json") # Sets Content-Type: application/json

# Set cookie with RFC 6265 directives
res.cookie("theme", "dark", {
    "maxAge":   86400 * 30,
    "httpOnly": True,
    "secure":   True,
    "sameSite": "Lax"
})

# Delete cookie
res.clearCookie("theme")

# Redirects
res.redirect("/login")
res.back("/fallback")
```

---

## 8. Built-in Security Middleware

### CORS Middleware (`kasbah.cors`)

Enable Cross-Origin Resource Sharing with support for dynamic origins, credentials, allowed headers, and preflight `OPTIONS` resolution:

```djazair
use kasbah

let app = new kasbah.app()

# 1. Default (Wildcard all origins '*')
app.middleware(kasbah.cors())

# 2. Advanced Enterprise CORS Configuration
app.middleware(kasbah.cors({
    "origin":            ["https://app.example.com", "https://admin.example.com"],
    "methods":           ["GET", "POST", "PUT", "DELETE", "PATCH"],
    "allowedHeaders":    ["Content-Type", "Authorization", "X-Requested-With"],
    "exposedHeaders":    ["Content-Range", "X-Total-Count"],
    "credentials":       True,
    "maxAge":            86400, # Cache preflight for 24h
    "preflightContinue": False
}))
```

> [!NOTE]
> When `credentials: True` is configured, Kasbah automatically reflects the specific request `Origin` and sets `Vary: Origin`, ensuring full compliance with W3C/RFC CORS specifications (as browsers reject wildcard `*` with credentials).

---

### Helmet Security Headers (`kasbah.helmet`)

Secure your HTTP response headers against common web vulnerabilities (Clickjacking, MIME-sniffing, XSS, and Information Disclosure):

```djazair
use kasbah

let app = new kasbah.app()

# 1. Default Security Stack
# Applies X-Frame-Options: SAMEORIGIN, X-Content-Type-Options: nosniff,
# X-XSS-Protection, HSTS (on HTTPS), Referrer-Policy, and removes X-Powered-By.
app.middleware(kasbah.helmet())

# 2. Custom Security Headers & Content Security Policy (CSP)
app.middleware(kasbah.helmet({
    "xFrameOptions":  "DENY",
    "referrerPolicy": "strict-origin-when-cross-origin",
    "hsts": {
        "maxAge":            31536000, # 1 year
        "includeSubDomains": True,
        "preload":           True
    },
    "csp": {
        "default-src": ["'self'"],
        "script-src":  ["'self'", "https://cdn.jsdelivr.net"],
        "style-src":   ["'self'", "'unsafe-inline'"],
        "img-src":     ["'self'", "data:", "https:"]
    }
}))
```

---

### Static File Serving (`kasbah.serveStatic`)

Serves assets with directory traversal protection, `Cache-Control` caching policies, and `index.html` fallback:

```djazair
app.middleware(kasbah.serveStatic("./public"))
```

---

### Structured Logging (`kasbah.logger`)

Outputs formatted request timing, status codes, and latency in milliseconds:

```djazair
app.middleware(kasbah.logger({
    "level":  "info", # "debug" | "info" | "warn" | "error" | "none"
    "format": "combined" # "dev" | "combined"
}))
```

---

## 9. Session & Cookie Management

Kasbah provides secure server-side file-backed sessions with signed HMAC-SHA256 tokens:

```djazair
let app = new kasbah.app({
    "session": {
        "secret":   "my-ultra-secure-cryptographic-key-32-chars-long!",
        "maxAge":   86400,
        "sameSite": "Lax"
    }
})

app.get("/visit", fn(req, res)
    # Session API
    let visits = req.session.get("visits", 0) + 1
    req.session.set("visits", visits)
    req.session.set("user", {"name": "Riad"})

    res.json({
        "sessionId":    req.session.id(),
        "isNewSession": req.session.isNew(),
        "visits":       visits
    })
end)

app.get("/logout", fn(req, res)
    req.session.clear() # Destroys session data on server
    res.noContent()
end)
```

---

## 10. Input Validation Engine (`kasbah.validator`)

Fluent, declarative schema validation with constraint chaining:

```djazair
app.post("/api/register", fn(req, res)
    let v = req.validate()

    v.field("username").required().string().minLength(3).maxLength(30)
    v.field("email").required().email()
    v.field("age").required().number().min(18).max(120)
    v.field("role").required().oneOf(["admin", "user", "editor"])
    v.field("website").optional().url()
    v.field("customCode").optional().matches("^[A-Z]{3}-[0-9]{4}$")
    v.field("agreement").required().boolean()

    # Custom rule closure
    v.field("points").optional().custom(fn(val)
        return if val % 10 == 0 ? True else "must be a multiple of 10"
    end)

    if v.fails()
        return res.validationError(v.errors())
    end

    let cleanData = v.validated()
    res.created({"status": "registered", "user": cleanData})
end)
```

---

## 11. File Uploads & File Manager

### Single File Upload via `req.file()`

```djazair
app.post("/upload/avatar", fn(req, res)
    if not req.hasFile("avatar")
        return res.badRequest("No avatar uploaded")
    end

    let file = req.file("avatar")

    # Validate image extension and size
    if not file.isImage() or not file.hasExt(["jpg", "png", "webp"])
        return res.badRequest("Avatar must be a JPG, PNG, or WebP image")
    end

    # Save to storage directory
    let savedPath = file.saveTo("./storage/avatars")
    res.ok({"path": savedPath, "size": file.getSize()})
end)
```

### Advanced Multi-File Upload via `uploader()`

```djazair
app.post("/upload/gallery", fn(req, res)
    let uploader = req.uploader({
        "dest":         "./storage/gallery",
        "maxSize":      5, # Max 5 MB per file
        "allowedTypes": ["image/jpeg", "image/png", "image/webp"],
        "prefix":       "gallery_"
    })

    let result = uploader.multiple("photos", 10)
    if not result["ok"]
        return res.badRequest(result["errors"])
    end

    res.created({"uploaded": result["files"]})
end)
```

---

## 12. Template Engine Integration (`Qalam`)

Kasbah seamlessly integrates with the **Qalam** AST template engine for server-side HTML rendering:

```djazair
let app = new kasbah.app({
    "views": "./views"
})

# Renders views/trades/index.html
app.get("/trades", fn(req, res)
    let trades = [
        {"id": 1, "symbol": "EURUSD", "profit": "+240$"},
        {"id": 2, "symbol": "BTCUSD", "profit": "+1,200$"}
    ]

    res.view("trades/index", {
        "title":  "Active Trades",
        "trades": trades
    })
end)
```

### Template Example (`views/trades/index.html`)

```html
{% extends "layouts/main" %}

{% block content %}
  <h1>{{ title }}</h1>
  <ul>
    {% for trade in trades %}
      <li><strong>{{ trade.symbol }}</strong>: {{ trade.profit }}</li>
    {% endfor %}
  </ul>
{% endblock %}
```

---

## 13. Centralized Error Handling

Global unhandled exception dispatcher:

```djazair
# 1. Custom 404 Not Found Handler
app.onNotFound(fn(req, res)
    if req.isJson()
        res.notFound("The requested resource was not found on this server.")
    else
        res.status(404).html("<h1>404 — Page Not Found</h1>")
    end
end)

# 2. Global Exception Handler (500)
app.onError(fn(err, req, res)
    print("[ERROR] " + req.method + " " + req.path + " -> " + str(err))
    
    if res.sentFlag return end

    res.status(500).json({
        "error":     "Internal Server Error",
        "timestamp": str(datetime.now())
    })
end)
```

---

## 14. Full REST Application Example

```djazair
use kasbah

let app = new kasbah.app({
    "port":          8080,
    "static":        "public",
    "bodyParser":    True,
    "logger":        True,
    "session":       True,
    "sessionSecret": "kasbah-production-secret-key-32-chars-long!"
})

# Security Middlewares
app.middleware(kasbah.helmet())
app.middleware(kasbah.cors({
    "origin":      "*",
    "credentials": True
}))

# In-Memory Database
let items = []

# API Routes
app.get("/api/items", fn(req, res)
    res.ok({"count": items.length(), "items": items})
end)

app.post("/api/items", fn(req, res)
    let v = req.validate()
    v.field("title").required().string().minLength(2)
    v.field("price").required().number().min(0)

    if v.fails()
        return res.validationError(v.errors())
    end

    let newItem = {
        "id":    items.length() + 1,
        "title": req.input("title"),
        "price": req.input("price")
    }
    items.append(newItem)

    res.created({"message": "Item created", "item": newItem})
end)

# Start Application Server
app.listen()
```

---

## 15. Running Tests

Kasbah contains a 100% passing automated test suite covering all modules:

```bash
djazair tests/test_routing.dz
djazair tests/test_validator.dz
djazair tests/test_session.dz
djazair tests/test_body_parser.dz
djazair tests/test_cors.dz
djazair tests/test_helmet.dz
djazair tests/test_kasbah_security.dz
djazair tests/test_kasbah_config.dz
djazair tests/test_view.dz
djazair tests/test_extreme_stress.dz
```

---

## 16. License

Kasbah is open-source software licensed under the **MIT License**.  
Developed and maintained by **Harizi Riyadh** (<hariziriyadh@gmail.com>).
