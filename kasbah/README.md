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

let app = new kasbah.Kasbah({"port": 8080})

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

## 3. Kasbah (Application Class)

### `new Kasbah(config)`

Creates a new application instance. `config` is a map of options:

| Option | Default | Description |
|--------|---------|-------------|
| `port` | `3000` | TCP port to listen on |
| `bodyParser` | `False` | Enable JSON/form body parsing |
| `logger` | `False` | Enable request logging |
| `session` | `False` | Enable file-based sessions |
| `sessionSecret` | `""` | Secret key for session ID generation |
| `static` | `""` | Directory path for static file serving |
| `maxBodySize` | `1048576` | Max request body size in bytes |

```djazair
let app = new kasbah.Kasbah({
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

### `app.middleware(fn)`

Adds a middleware function that runs before route handlers:

```djazair
app.middleware(fn(req, res)
    if req.header("x-auth") != "admin"
        res.status(401).text("Unauthorized")
    end
end)
```

### `app.listen()`

Starts the HTTP server. Must be called last.

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

Internal route matching engine. Supports `:param` patterns and middleware chains. Created automatically by `Kasbah`.

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
let app = new kasbah.Kasbah({
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

let app = new kasbah.Kasbah({
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
    let text = req.body["text"]
    if isNull(text) || text == ""
        res.status(400).json({"error": "text field required"})
        return
    end
    let msg = {
        "id": messages.length(),
        "text": text,
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
| `01_hello.dz` | Minimal server, GET, JSON, redirect |
| `02_routing.dz` | Route params, query strings, POST body |
| `03_middleware.dz` | Custom middleware, auth, static files |
| `04_session.dz` | File-based sessions, visit counter |
| `05_full_app.dz` | Guestbook with all features combined |
