# Djazair Kasbah — Web Framework

A modular, object-oriented web framework for the Djazair programming language, built on the built-in `http` module. Supports routing with `:param` patterns, middleware chains, sessions, static file serving, and body parsing.

---

## 1. Installation

```bash
dpm install kasbah
```

Kasbah is a pure-Dz extension — no native build step needed.

---

## 2. Quick Start

```djazair
use kasbah

let app = new kasbah.KasbahApp({"port": 8080})

app.get("/", fn(req, res)
    res.text("Hello from Kasbah!")
end)

app.listen()
```

Run it:
```bash
djazair myserver.dz
```

---

## 3. KasbahApp (Application Class)

### `new KasbahApp(config)`

Creates a new application instance. `config` is a map of options:

| Option | Default | Description |
|--------|---------|-------------|
| `port` | `3000` | TCP port to listen on |
| `bodyParser` | `True` | Enable JSON/form body parsing |
| `logger` | `True` | Enable request logging |
| `session` | `False` | Enable file-based sessions |
| `sessionSecret` | `""` | Secret key for session ID generation |
| `static` | `""` | Directory path for static file serving |
| `maxBodySize` | `1048576` | Max request body size in bytes (configurable per-app) |

```djazair
let app = new kasbah.KasbahApp({
    "port": 8080,
    "bodyParser": True,
    "logger": True,
    "session": True,
    "sessionSecret": "my-secret",
    "static": "./public"
})
```

### `app.get(path, handler)`
### `app.post(path, handler)`
### `app.put(path, handler)`
### `app.delete(path, handler)`
### `app.patch(path, handler)`

Register a route handler for the given HTTP method and path. Supports `:param` segments:

```djazair
app.get("/users/:id", fn(req, res)
    res.json({"userId": req.param("id")})
end)

app.get("/users/:id/posts/:postId", fn(req, res)
    res.json({"user": req.param("id"), "post": req.param("postId")})
end)
```

### `app.group(prefix, callback)`

Groups routes under a common path prefix with shared middleware.

```djazair
app.group("/admin", fn(g)
    g.middleware(authMw)
    g.get("/", adminHome)       # → GET /admin
    g.get("/users", listUsers)  # → GET /admin/users
end)
```

### `app.middleware(fn)`

Adds a middleware function that runs before route handlers:

```djazair
app.middleware(fn(req, res)
    if req.header("x-auth") != "admin"
        res.status(401).text("Unauthorized")
    end
end)
```

### `app.listen(port = Null, quiet = False)`

Builds the middleware pipeline and starts the HTTP server. Must be called last.

- `port` — overrides the configured port for this call only.
- `quiet = True` — suppresses the startup message.

### `app.onError(handler)`

Registers a global error handler called when any unhandled exception occurs inside a middleware or route handler. The handler receives `(err, req, res)`.

```djazair
app.onError(fn(err, req, res)
    print("[ERROR] ${req.method} ${req.path} -> ${str(err)}")
    if !res._sent
        res.status(500).json({"error": "Internal Server Error", "message": str(err)})
    end
end)
```

### `app.close()`

Stops the server and cleans up sessions (if enabled).

---

## 4. Request Class

Wraps the built-in `HttpRequest`. Available inside route/middleware handlers.

### `req.param(name)`

Returns a named route parameter (from `:param` segments). Returns `Null` if not found.

```djazair
# Route: /users/:id
let id = req.param("id")
```

### `req.query(name = Null)`

Returns query string parameter by name. If called with no arguments, returns the full query map.

```djazair
# ?q=hello&page=2
let q = req.query("q")       # "hello"
let all = req.query()        # {"q": "hello", "page": "2"}
```

### `req.header(name)`

Returns a request header value (case-insensitive). Returns `""` if missing.

```djazair
let ct = req.header("content-type")
```

### `req.body`

The parsed request body (map from JSON, or raw string). Available when `bodyParser` is enabled.

```djazair
let name = req.body["name"]
```

### `req.files`

A map containing uploaded files when parsing `multipart/form-data` requests. Available when `bodyParser` is enabled.

Files are written to a secure temporary directory (`os.tmpDir()/kasbah_uploads/`) while the request is being handled, and automatically deleted after the response is sent. Move them to a permanent location inside your handler using `file.move()`.

**Security:** The `filename` field is automatically sanitized — all path separators are stripped — so `"../../etc/passwd"` becomes `"passwd"`. You can safely use `uploaded["filename"]` when constructing destination paths.

**413 Payload Too Large:** If an individual file exceeds `maxBodySize` the framework immediately responds with HTTP 413 and the upload is rejected.

```djazair
app.post("/upload", fn(req, res)
    let uploaded = req.files["avatar"]
    if isNull(uploaded)
        res.status(400).text("Missing file")
        return
    end

    print("File: ${uploaded["filename"]}, Size: ${uploaded["size"]}")

    # filename is already sanitized — safe to use as a path component
    file.move(uploaded["tempPath"], "./uploads/" + uploaded["filename"])

    res.json({"status": "uploaded", "name": uploaded["filename"]})
end)
```


### `req.cookie(name)`

Returns a cookie value by name. Returns `Null` if not found.

### `req.get(key)`

Returns a custom attribute set by middleware via `req.set()`.

```djazair
# In middleware:
req.set("startTime", datetime.timestamp())

# In handler:
let start = req.get("startTime")
```

### `req.set(key, value)`

Stores a custom attribute on the request object. Available to all downstream handlers.

### `req.inputs()`

Returns a merged map of `params` + `query` + `body` (body has highest priority).

```djazair
let all = req.inputs()
```

### `req.input(name, fallback = Null)`

Looks up a value from inputs in order: params → query → body. Returns `fallback` if not found.

```djazair
let name = req.input("name")       # from body, query, or params
let page = req.input("page", 1)    # with fallback
```

### `req.only(keys)`

Returns a subset of inputs containing only the specified keys.

```djazair
let safe = req.only(["name", "email"])
```

### `req.except(keys)`

Returns all inputs except the specified keys.

```djazair
let clean = req.except(["token", "_sid"])
```

### `req.isAjax()`

Returns `True` if the `X-Requested-With` header equals `XMLHttpRequest`.

```djazair
if req.isAjax()
    res.json(data)
end
```

### `req.isMethod(method)`

Returns `True` if the request method matches (case-insensitive).

```djazair
if req.isMethod("post")
    # handle POST
end
```

### `req.accepts(type)`

Checks if the client's `Accept` header includes the given content type.

```djazair
if req.accepts("application/json")
    res.json(data)
end
```

### `req.session`

The session data map. Only available when `session` config is enabled. Automatically persisted between requests.

```djazair
if !req.session.has("visits")
    req.session["visits"] = 0
end
req.session["visits"] = req.session["visits"] + 1
```

---

## 5. Response Class

Wraps the built-in `HttpResponse`. All methods support method chaining (fluent API).

### `res.status(code)`

Sets the HTTP status code. Returns self for chaining.

```djazair
res.status(201).json({"created": True})
```

### `res.setHeader(name, value)`

Sets a response header. Rejects `\r` and `\n` to prevent HTTP response splitting.

```djazair
res.setHeader("x-powered-by", "Kasbah")
```

### `res.send(body)`

Sends a raw string body. Does NOT set Content-Type.

### `res.text(body)`

Sets `Content-Type: text/plain` and sends the body.

```djazair
res.text("Hello World")
```

### `res.html(body)`

Sets `Content-Type: text/html` and sends the body.

```djazair
res.html("<h1>Hello</h1>")
```

### `res.json(data)`

Serializes a map to JSON, sets `Content-Type: application/json`, and sends.

```djazair
res.json({"message": "ok", "count": 5})
```

### `res.file(path)`

Sends a file from disk. Sets Content-Type based on file extension.

```djazair
res.file("./static/image.png")
```

### `res.download(path, filename = Null)`

Sends a file as an attachment download. Optional custom filename.

```djazair
res.download("./report.pdf", "monthly-report.pdf")
```

### `res.redirect(url, code = 302)`

Redirects the client to the given URL.

```djazair
res.redirect("/login")
res.redirect("https://example.com", 301)
```

### `res.cookie(name, value, options)`

Sets a cookie with optional attributes.

```djazair
res.cookie("token", "abc123", {
    "httpOnly": True,
    "maxAge": 3600,
    "path": "/",
    "sameSite": "lax"
})
```

### `res.deleteCookie(name)`

Deletes a cookie by setting `Max-Age=0` and an expired date.

```djazair
res.deleteCookie("token")
```

### `res.type(ext)`

Sets `Content-Type` based on file extension using the internal MIME map.

```djazair
res.type(".css").send("body { color: red }")
```

### `res.links(linkMap)`

Builds a `Link` header from a map of rel → URL pairs.

```djazair
res.links({"next": "/page/2", "prev": "/page/1"})
# Link: </page/2>; rel="next", </page/1>; rel="prev"
```

---

## 6. Router Class

Internal route matching engine. Supports `:param` patterns and middleware chains. Created automatically by `KasbahApp`.

```djazair
let router = new kasbah.Router()
router.get("/users/:id", fn(req, res) ... end)
router.handle(req, res)   # returns True if a route matched
```

---

## 7. Middleware Reference

### BodyParser

Parses `application/json` and `application/x-www-form-urlencoded` request bodies. Enabled via `"bodyParser": True` config. Respects configurable `maxBodySize`.

### Logger

Logs method, path, status code, and duration for each request. Enabled via `"logger": True`.

```
GET /index.html 200 0.0012ms
```

### Session

File-based session storage. Each session is a JSON file in a temp directory. The session ID is set via a signed cookie. Enabled via `"session": True` and `"sessionSecret"`.

```djazair
let app = new kasbah.KasbahApp({
    "session": True,
    "sessionSecret": "your-secret-key"
})
```

### Static

Serves files from a directory for GET requests. Prevents path traversal. Enabled via `"static": "./public"`.

```
public/
├── index.html
├── style.css
└── images/
    └── logo.png
```

Accessible at `/index.html`, `/style.css`, `/images/logo.png`.

---

## 8. Helper Functions

### `merge(base, override)`

Deep-merges two maps. Used internally for config merging.

### `generateId(len)`

Generates a random hex string of the given length. Used for session IDs.

### `mimeType(ext)`

Returns the MIME type for a file extension.

```djazair
let ct = mimeType(".html")   # "text/html"
let ct = mimeType(".json")   # "application/json"
```

### `parseUrl(url)`

Parses a URL string into its components (path, query string, query map).

### `parseContentType(header)`

Parses a Content-Type header into media type and parameters.

### `parseCookie(header)`

Parses a Cookie header string into a map of name → value.

### `serializeCookie(name, value, options)`

Serializes a cookie name/value pair with options into a `Set-Cookie` header string.

---

## 9. Complete Example

```djazair
use kasbah
use datetime

let app = new kasbah.KasbahApp({
    "port": 8080,
    "bodyParser": True,
    "logger": True,
    "session": True,
    "sessionSecret": "guestbook-secret"
})

let messages = []

app.middleware(fn(req, res)
    req.set("timestamp", datetime.timestamp())
end)

app.get("/", fn(req, res)
    res.text("Welcome to the Guestbook")
end)

app.post("/messages", fn(req, res)
    let data = req.only(["text", "author"])
    if isNull(data["text"]) || data["text"] == ""
        res.status(400).json({"error": "text field required"})
        return
    end
    let msg = {
        "id": messages.length(),
        "text": data["text"],
        "author": data["author"] || "Anonymous",
        "time": req.get("timestamp")
    }
    messages.append(msg)
    res.status(201).json({"created": msg})
end)

app.get("/messages/:id", fn(req, res)
    let idx = int(req.param("id"))
    if idx >= messages.length()
        res.status(404).json({"error": "not found"})
        return
    end
    res.json(messages[idx])
end)

app.get("/session", fn(req, res)
    if !req.session.has("visits")
        req.session["visits"] = 0
    end
    req.session["visits"] = req.session["visits"] + 1
    res.json({"visits": req.session["visits"]})
end)

app.listen()
```

---

## 10. Examples

See the [`examples/`](examples/) directory for runnable demos:

| File | Features |
|------|----------|
| `01_hello.dz` | Text, JSON, sendStatus, custom headers via header() setter |
| `02_routing.dz` | Route params, input()/inputs()/only()/except() helpers |
| `03_middleware.dz` | Custom middleware, auth, static files, isAjax() |
| `04_session.dz` | File-based sessions, visit counter |
| `05_full_app.dz` | Guestbook with all features combined |
| `06_error_handler.dz` | Global error handling with `onError`, sendStatus() |
| `07_route_groups.dz` | Route groups, shared middleware, isAjax(), header() setter, input() |
