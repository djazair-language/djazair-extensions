# Kasbah — Web Framework for Djazair

**Version: 0.0.2**  
**Author: Harizi Riyadh (<hariziriyadh@gmail.com>)**

Kasbah is a modular, fast, and secure web framework for the Djazair programming language. It is built on top of Djazair's native `http` module and provides a high-level, fluent API for routing, middleware, sessions, static file serving, and view rendering.

## Key Features
- **Expressive Routing**: Elegant, native routing with URL parameter extraction (`:id`).
- **Secure by Default**: Built-in protections against Path Traversal vulnerabilities in static files and view templates.
- **Robust Error Handling**: Absolute error boundaries prevent the server from crashing, even on malformed HTTP requests.
- **Automatic Decoding**: URLs, query parameters, and form bodies are automatically URL-decoded so you don't have to deal with `%20`.
- **Smart Views**: Raw template rendering with auto-appended `.html` and `{{key}}` injection.
- **Multipart Uploads**: Safe file uploads with automatic path sanitization and configurable size limits.
- **Developer Friendly**: Uses standard `camelCase` naming conventions globally.

---

## 1. Installation

```bash
dpm install kasbah
```

Kasbah is a pure-Djazair extension — no native build step needed.

---

## 2. Quick Start

```djazair
use kasbah

let app = new kasbah.kasbahApp({"port": 3000})

app.get("/", fn(req, res)
    res.html("<h1>Welcome to Kasbah!</h1>")
end)

app.get("/hello/:name", fn(req, res)
    res.json({
        "message": "Hello, " + req.param("name")
    })
end)

app.listen()
```

Run the server:
```bash
djazair server.dz
```

---

## 3. Configuration

When creating the application, you can pass a configuration map to `new kasbah.kasbahApp(config)`.

| Option | Default | Description |
|--------|---------|-------------|
| `port` | `3000` | TCP port to listen on. |
| `host` | `"0.0.0.0"` | Bind address. |
| `bodyParser` | `True` | Automatically parse JSON and Form Data bodies. |
| `maxBodySize` | `1048576` | Maximum size (in bytes) allowed for an uploaded file (Default: 1MB). |
| `logger` | `True` | Enable the built-in request/error logger. |
| `session` | `False` | Enable file-based persistent sessions. |
| `sessionSecret` | `"kasbah"` | Secret used to sign session cookies. |
| `static` | `""` | Absolute or relative path to a directory for serving static files. |
| `views` | `"views"` | Directory path containing your HTML templates. |

---

## 4. Routing (`kasbahApp`)

Routing is handled natively inside the `kasbahApp` class. Paths and variables are automatically URL-decoded, and trailing slashes are ignored (e.g., `/users/` is treated as `/users`).

### `app.get(path, handler)`
### `app.post(path, handler)`
### `app.put(path, handler)`
### `app.delete(path, handler)`
### `app.patch(path, handler)`
### `app.all(path, handler)`

Register a route handler for the given HTTP method and path. Supports `:param` segments:

```djazair
app.get("/users/:id/posts/:postId", fn(req, res)
    let userId = req.param("id")
    let postId = req.param("postId")
    res.json({"user": userId, "post": postId})
end)
```
*Note: If the client visits `/users/john%20doe/posts/1`, `req.param("id")` will automatically decode to `"john doe"`.*

### `app.middleware(fn)`
Adds a middleware function that runs before route handlers.

### `app.onError(handler)`
Registers a global error handler called when any unhandled exception occurs inside a middleware or route handler. The handler receives `(err, req, res)`.

### `app.listen(port = Null, quiet = False)`
Starts the HTTP server. Must be called last.

### `app.close()`
Stops the server and cleans up sessions (if enabled).

---

## 5. Request (`request`) Object

The `request` object wraps the incoming HTTP request and provides safe, high-level methods to access inputs.

### Properties
- `req.method`: The HTTP method (e.g., "GET").
- `req.path`: The normalized, URL-decoded request path (e.g., "/api/users").
- `req.ip`: The client's IP address (supports Reverse Proxy headers like `x-forwarded-for`).
- `req.rawHeaders`: The original HTTP headers map.
- `req.body`: The parsed request body (JSON map or form data map).
- `req.files`: Uploaded files (if `multipart/form-data`).
- `req.session`: The active session map (if sessions are enabled).
- `req.cookies`: Map of parsed cookies.

### Parameter Extractors

#### `req.param(name)`
Get a route parameter. Returns `Null` if missing.

#### `req.query(name = Null)`
Get a query string parameter. Automatically URL-decoded. If called with no arguments, returns the full query map.

#### `req.header(name)`
Get a header value (case-insensitive). Returns `""` if missing.

#### `req.cookie(name)`
Get a parsed cookie value. Returns `Null` if not found.

### Input Management (Form/Body/Query/Params)

#### `req.inputs()` 
Merges inputs from `params`, `query`, and `body` into a single map. `body` has the highest priority.

#### `req.input(name, fallback = Null)`
Looks up a value from inputs in order: params → query → body. Returns `fallback` if not found.

#### `req.only(keys)` 
Returns a subset of inputs containing only the specified keys.
```djazair
let safeData = req.only(["username", "password"])
```

#### `req.except(keys)`
Returns all inputs except the specified keys.

### Request Context & Checking

#### `req.get(key)`
Returns a custom attribute set by middleware via `req.set()`.

#### `req.set(key, value)`
Stores a custom attribute on the request object. Available to all downstream handlers.

#### `req.isAjax()`
Returns `True` if the `X-Requested-With` header equals `XMLHttpRequest`.

#### `req.isMethod(method)`
Returns `True` if the request method matches (case-insensitive).

#### `req.accepts(type)`
Checks if the client's `Accept` header includes the given content type.

---

## 6. Response (`response`) Object

The `response` object is used to send data to the client. All mutating methods support chaining.

### HTTP Status & Headers

#### `res.status(code)`
Set the HTTP status code. Returns self for chaining.

#### `res.setHeader(name, value)`
Set a custom response header. Rejects `\r` and `\n` to prevent HTTP response splitting.

#### `res.type(ext)`
Sets `Content-Type` based on file extension using the internal MIME map (e.g., `res.type(".css")`).

#### `res.links(linkMap)`
Builds a `Link` header from a map of rel → URL pairs.

### Cookies

#### `res.cookie(name, value, options)`
Set a cookie with security attributes (`maxAge`, `httpOnly`, `path`, `domain`, `secure`, `sameSite`).
```djazair
res.cookie("token", "123", {"httpOnly": True, "maxAge": 3600})
```

#### `res.deleteCookie(name)`
Deletes a cookie by setting `Max-Age=0` and an expired date.

### Sending Responses

#### `res.send(body)`
Sends a raw string body. Does NOT set Content-Type automatically.

#### `res.text(body)`
Sets `Content-Type: text/plain` and sends the body.

#### `res.html(body)` 
Sets `Content-Type: text/html` and sends the body.

#### `res.json(data)`
Serializes a Djazair map or array to JSON, sets `Content-Type: application/json`, and sends.

#### `res.view(templateName, data = {})`
Render a template from the configured `views` directory. If the extension is missing, `.html` is automatically appended. Placeholders matching `{{key}}` or `{{ key }}` will be replaced with values from the `data` map.
```djazair
# Resolves to views/profile.html
res.view("profile", {"username": "Admin"}) 
```

#### `res.file(path)`
Serve a static file from disk. MIME types are automatically resolved.

#### `res.download(path, filename = Null)`
Force the browser to download a file. Optionally set a custom filename.

#### `res.redirect(url, code = 302)`
Redirect the client to the given URL.

---

## 7. Built-in Features

### File Uploads (`req.files`)
When `bodyParser` is enabled, `multipart/form-data` uploads are safely parsed to `req.files`.
- **Size Limits**: Exceeding `maxBodySize` immediately triggers a `413 Payload Too Large` error.
- **Auto-Cleanup**: Temporary files are deleted automatically at the end of the request if not explicitly moved via `file.move()`.

### Sessions (`req.session`)
File-based session storage. Each session is a JSON file in a temp directory. The session ID is set via a signed cookie. Enabled via `"session": True` and `"sessionSecret"`.
```djazair
if !req.session.has("visits")
    req.session["visits"] = 0
end
req.session["visits"] = req.session["visits"] + 1
```

### Static File Server
When configuring `"static": "./public"`, Kasbah acts as a secure static file server, serving files directly from the public directory.

### Logger
Logs requests and unhandled errors. Output format:
```
[12:00:01] [INFO] GET /index.html 200 0.0012ms
[12:00:05] [ERROR] POST /api/data -> NameError: undefined variable
```

### Error Handling
Kasbah catches all errors gracefully. Exceptions in middlewares, route handlers, or during file uploads will not crash the server. Instead, an `[ERROR]` is logged, and a `500 Internal Server Error` is sent to the client.

To customize error handling:
```djazair
app.onError(fn(err, req, res)
    res.status(500).view("errors/500", {"error": str(err)})
end)
```

---

## 8. Full Example

```djazair
use kasbah
use file

let app = new kasbah.kasbahApp({
    "port": 3000,
    "static": "./public",
    "views": "./templates",
    "session": True,
    "sessionSecret": "super-secret"
})

# Global Middleware
app.middleware(fn(req, res)
    print("Incoming Request: " + req.path)
end)

# View Rendering
app.get("/", fn(req, res)
    res.view("index", {"title": "Home Page"})
end)

# JSON API
app.get("/api/data", fn(req, res)
    res.json({"status": "active", "version": "1.0"})
end)

# File Uploads
app.post("/upload", fn(req, res)
    let uploaded = req.files["document"]
    if isNull(uploaded)
        res.status(400).json({"error": "No file uploaded"})
        return
    end

    let dest = "./storage/" + uploaded["filename"]
    file.move(uploaded["tempPath"], dest)

    res.json({"status": "success", "savedTo": dest})
end)

# Start the server
app.listen()
```
