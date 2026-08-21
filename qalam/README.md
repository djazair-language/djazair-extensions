# Qalam (قلم) — Modular, High-Performance HTML Template Engine for Djazair

<p align="center">
  <img src="https://raw.githubusercontent.com/djazair-language/djazair-extensions/main/qalam/assets/logo.png" alt="Qalam Logo" width="120" onerror="this.style.display='none'"/>
</p>

<p align="center">
  <strong>A fast, secure, AST-based HTML template engine designed exclusively for the <a href="https://github.com/djazair-language">Djazair Programming Language</a>.</strong>
</p>

<p align="center">
  <a href="./dpm.json"><img src="https://img.shields.io/badge/version-0.2.0-blue.svg?style=flat-square" alt="Version 0.2.0"></a>
  <a href="./LICENSE"><img src="https://img.shields.io/badge/license-MIT-green.svg?style=flat-square" alt="License MIT"></a>
  <a href="https://github.com/djazair-language"><img src="https://img.shields.io/badge/language-Djazair-red.svg?style=flat-square" alt="Djazair Language"></a>
  <a href="./tests/run_all_tests.dz"><img src="https://img.shields.io/badge/tests-88%20passed-brightgreen.svg?style=flat-square" alt="Tests Passed"></a>
  <a href="./docs/index.html"><img src="https://img.shields.io/badge/docs-interactive%20html-cyan.svg?style=flat-square" alt="Documentation"></a>
</p>

---

## 📖 Table of Contents

- [Overview & Etymology](#-overview--etymology)
- [Key Features](#-key-features)
- [Architecture & Design Principles](#-architecture--design-principles)
- [Installation](#-installation)
- [Quick Start](#-quick-start)
  - [In-Memory Rendering](#1-in-memory-rendering-no-disk-io-needed)
  - [File-Based Rendering](#2-file-based-rendering-with-views-directory)
  - [Integration with Kasbah Framework](#3-integration-with-kasbah-web-framework)
- [Template Syntax Guide](#-template-syntax-guide)
  - [Variable Interpolation & Auto-Escaping](#variable-interpolation--auto-escaping)
  - [Raw Output](#raw-output)
  - [Filter Pipelines & Transformations](#filter-pipelines--transformations)
  - [Conditionals & Multi-Branching](#conditionals--multi-branching)
  - [Logical & Comparison Operators](#logical--comparison-operators)
  - [Loops & Loop Context Metadata](#loops--loop-context-metadata)
  - [Template Inheritance & Blocks](#template-inheritance--blocks)
  - [Partials & Sub-Templates](#partials--sub-templates)
  - [Template Comments](#template-comments)
  - [Whitespace Control](#whitespace-control)
- [Built-In Filter Catalog](#-built-in-filter-catalog)
- [Custom Filter Registration](#-custom-filter-registration)
- [API Reference](#-api-reference)
  - [Class: `qalam.View`](#class-qalamview)
  - [Helper: `qalam.renderString()`](#helper-qalamrenderstring)
  - [Security: `qalam.escapeHtml()`](#security-qalamescapehtml)
- [Security Model](#-security-model)
- [Performance & AST Caching](#-performance--ast-caching)
- [Testing & Quality Assurance](#-testing--quality-assurance)
- [License & Author](#-license--author)

---

## 🖋️ Overview & Etymology

> **قلم** *(Qalam, noun, Arabic)* — The pen; the classical instrument of writing, structure, and expression.

**Qalam** is a production-ready, standalone HTML template engine built with clean modular architecture for the **Djazair Programming Language**. Inspired by the expressive power and reliability of world-class template engines like *Jinja*, *Twig*, *Blade*, and *Liquid*, Qalam combines an intuitive tag syntax with high-performance Lexical scanning, Abstract Syntax Tree (AST) parsing, robust execution, and memory caching.

---

## ⚡ Key Features

- 🛡️ **Zero-Configuration Security:** All `{{ var }}` outputs are HTML-escaped by default to prevent Cross-Site Scripting (XSS).
- 🧩 **Modular Layered Architecture:** Clear separation of concerns into dedicated components (`Lexer`, `Parser`, `Evaluator`, `Loader`, `Filters`, `Escaper`).
- ⚡ **In-Memory AST Caching:** Compiles templates into AST trees once and reuses them across requests for maximum throughput.
- 🔀 **Filter Pipeline (Piping):** Transform data easily with UNIX-style piping: `{{ name | trim | capitalize }}`.
- 🌿 **Extensible Filter Registry:** Register custom transformation functions at runtime.
- 🌳 **Multi-Branch Conditionals:** Full support for `{% if %}`, `{% elif %}`, `{% else if %}`, and `{% else %}`.
- 🔄 **Rich Loop Context (`loop`):** Access `loop.index`, `loop.first`, `loop.last`, `loop.length`, `loop.even`, `loop.odd`, and `{% else %}` fallback.
- 🏗️ **Template Inheritance:** Modular layouts with `{% extends "layout" %}` and overrideable `{% block name %}` regions.
- 📦 **Partials & Sub-Views:** Embed reusable components via `{% include "partials/nav" %}`.
- 🧹 **Whitespace Stripping & Comments:** Clean outputs using `{{- -}}` / `{%- -%}` and silent `{# comment #}` tags.
- 🔒 **Path Traversal Defense:** Strict directory boundary validation prevents unauthorized filesystem access.
- 🔄 **100% Backward Compatible:** Drop-in replacement for existing Qalam and Kasbah projects.

---

## 🏛️ Architecture & Design Principles

Qalam is engineered according to **SOLID** principles and Clean Architecture. Each phase of the rendering pipeline is encapsulated in a specialized module:

```
┌─────────────────┐       ┌─────────────────┐       ┌─────────────────┐
│ Template Source │ ────▶ │  Lexer (Tokens) │ ────▶ │   Parser (AST)  │
└─────────────────┘       └─────────────────┘       └─────────────────┘
                                                             │
                                                             ▼
┌─────────────────┐       ┌─────────────────┐       ┌─────────────────┐
│ Rendered Output │ ◀──── │ Escaper / Safe  │ ◀──── │ Evaluator Engine│ ◀─── AST Cache
│      HTML       │       │   Encoding      │       │(Context+Filters)│
└─────────────────┘       └─────────────────┘       └─────────────────┘
```

| Module | Location | Responsibility |
|---|---|---|
| `constants.dz` | `core/constants.dz` | Type constants for Tokens (`TOKEN_VAR`, `TOKEN_TAG`...) and AST Nodes. |
| `escaper.dz` | `core/escaper.dz` | High-speed HTML entity encoding (`&`, `<`, `>`, `"`, `'`). |
| `lexer.dz` | `core/lexer.dz` | Tokenizes source code into tokens, handles comments and whitespace. |
| `parser.dz` | `core/parser.dz` | Builds hierarchical AST node trees with syntax validation. |
| `filters.dz` | `core/filters.dz` | Manages built-in transformations and user-registered custom filters. |
| `evaluator.dz` | `core/evaluator.dz` | Traverses AST nodes, executes boolean logic, filters, and loop contexts. |
| `loader.dz` | `core/loader.dz` | Filesystem boundary verification and in-memory AST cache engine. |
| `view.dz` | `core/view.dz` | Public orchestrator class (`View`). |
| `engine.dz` | `core/engine.dz` | Backward-compatibility facade. |

---

## 📦 Installation

Install Qalam into your Djazair project using the official package manager:

```sh
dpm install qalam
```

Or import it directly in your project:

```djazair
use qalam
```

---

## 🚀 Quick Start

### 1. In-Memory Rendering (No disk I/O needed)

Ideal for emails, notifications, API responses, or code generation:

```djazair
use qalam

let template = `
<h1>Hello, {{ user.name | capitalize }}!</h1>
<p>Status: {% if user.isActive %}Active Member{% else %}Pending{% endif %}</p>
<ul>
{% for item in items %}
  <li>#{{ loop.index }}: {{ item }}</li>
{% endfor %}
</ul>
`

let data = {
    "user": {"name": "riad", "isActive": True},
    "items": ["Djazair Language", "Kasbah Framework", "Qalam Template Engine"]
}

let renderedHtml = qalam.renderString(template, data)
print(renderedHtml)
```

---

### 2. File-Based Rendering (With `views` directory)

Directory structure:
```text
my_project/
├── views/
│   ├── layouts/
│   │   └── base.html
│   ├── partials/
│   │   └── nav.html
│   └── home.html
└── main.dz
```

**Base layout** (`views/layouts/base.html`):
```html
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>{% block title %}My Application{% endblock %}</title>
</head>
<body>
    <header>{% include "partials/nav" %}</header>
    <main>
        {% block content %}{% endblock %}
    </main>
    <footer>&copy; 2026 Djazair Language</footer>
</body>
</html>
```

**Page template** (`views/home.html`):
```html
{% extends "layouts/base" %}

{% block title %}Dashboard — {{ siteName }}{% endblock %}

{% block content %}
    <h2>Welcome back, {{ user.name }}!</h2>
    <p>Your role: <span class="badge">{{ user.role | upper }}</span></p>
{% endblock %}
```

**Rendering from code** (`main.dz`):
```djazair
use qalam

let viewEngine = new qalam.View({
    "views": "views",
    "cache": True
})

let html = viewEngine.render("home", {
    "siteName": "Enterprise Portal",
    "user": {"name": "Riad", "role": "admin"}
})

print(html)
```

---

### 3. Integration with Kasbah Web Framework

Qalam is the native template engine powering the [Kasbah](https://github.com/djazair-language/djazair-extensions/tree/main/kasbah) web framework. `res.view()` integrates with Qalam automatically:

```djazair
use kasbah

let app = new kasbah.app({"views": "views"})

app.get("/profile", fn(req, res)
    res.view("profile", {
        "title": "User Profile",
        "user": req.session.get("user")
    })
end)

app.listen(8080)
```

---

## 📝 Template Syntax Guide

### Variable Interpolation & Auto-Escaping

Variables inside double curly braces `{{ ... }}` are evaluated and **automatically HTML-escaped** to prevent XSS:

```html
<p>Hello, {{ user.name }}</p>
<p>Escaped input: {{ "<script>alert(1)</script>" }}</p>
<!-- Output: &lt;script&gt;alert(1)&lt;/script&gt; -->
```

Supports **dot-notation** and **bracket indexing** for nested maps and arrays:
```html
<span>{{ user.profile.address.city }}</span>
<span>{{ articles[0].title }}</span>
```

---

### Raw Output

For trusted markup that must not be escaped, use triple curly braces `{{{ ... }}}`:

```html
<div class="article-body">
    {{{ article.trustedHtmlContent }}}
</div>
```

---

### Filter Pipelines & Transformations

Filters transform values using the pipe character (`|`). Multiple filters can be chained sequentially:

```html
<h1>{{ page.title | trim | title }}</h1>
<p>Joined tags: {{ tags | join(" • ") | upper }}</p>
<p>Price: ${{ product.price | round(2) }}</p>
<p>Bio: {{ user.bio | default("No biography provided.") }}</p>
```

---

### Conditionals & Multi-Branching

Control output conditionally using `{% if %}`, `{% elif %}` (or `{% else if %}`), and `{% else %}`:

```html
{% if user.role == 'admin' %}
    <span class="badge badge-danger">Administrator</span>
{% elif user.role == 'moderator' %}
    <span class="badge badge-warning">Moderator</span>
{% elif user.role == 'author' %}
    <span class="badge badge-info">Author</span>
{% else %}
    <span class="badge badge-secondary">Member</span>
{% endif %}
```

---

### Logical & Comparison Operators

Qalam supports complex logical expressions:

| Operator | Syntax | Description |
|---|---|---|
| Equality | `a == b` | Exact value equality |
| Inequality | `a != b` | Inequality check |
| Relational | `>`, `<`, `>=`, `<=` | Numeric comparisons |
| Logical AND | `condA and condB` | True if both conditions are truthy |
| Logical OR | `condA or condB` | True if at least one condition is truthy |
| Negation | `not cond` or `!cond` | Inverts condition truthiness |

```html
{% if user.isVerified and not user.isBanned %}
    <button class="btn-primary">Proceed to Checkout</button>
{% endif %}
```

#### Truthiness Rules
- **Falsy values:** `Null`, `False`, `0`, `0.0`, `""` (empty string), `[]` (empty array), `{}` (empty map).
- **Truthy values:** Any non-zero number, non-empty string, populated array, populated map, or `True`.

---

### Loops & Loop Context Metadata

Iterate over arrays using `{% for item in items %}`:

```html
<table>
  <thead>
    <tr>
      <th>#</th>
      <th>Product</th>
      <th>Price</th>
    </tr>
  </thead>
  <tbody>
  {% for item in products %}
    <tr class="{% if loop.even %}row-even{% else %}row-odd{% endif %}">
      <td>{{ loop.index }}</td>
      <td>{{ item.name }}</td>
      <td>${{ item.price | round(2) }}</td>
    </tr>
  {% else %}
    <tr><td colspan="3">No products found.</td></tr>
  {% endfor %}
  </tbody>
</table>
```

#### Automatic `loop` Context Object

| Property | Type | Description |
|---|---|---|
| `loop.index` | Number | 1-based index (`1, 2, 3...`) |
| `loop.index0` | Number | 0-based index (`0, 1, 2...`) |
| `loop.first` | Boolean | `True` during the first iteration |
| `loop.last` | Boolean | `True` during the last iteration |
| `loop.length` | Number | Total item count in the collection |
| `loop.even` | Boolean | `True` if current 1-based iteration is even |
| `loop.odd` | Boolean | `True` if current 1-based iteration is odd |

---

### Template Inheritance & Blocks

Create structured, DRY layouts with `{% extends %}` and `{% block %}`:

**Base layout** (`views/layout.html`):
```html
<!DOCTYPE html>
<html>
<head>
  <title>{% block title %}Default App{% endblock %}</title>
</head>
<body>
  <header>{% block header %}<nav>Main Navigation</nav>{% endblock %}</header>
  <main>{% block content %}{% endblock %}</main>
  <footer>{% block footer %}&copy; 2026 Djazair{% endblock %}</footer>
</body>
</html>
```

**Child template** (`views/page.html`):
```html
{% extends "layout" %}

{% block title %}Custom Page Title{% endblock %}

{% block content %}
  <h1>Welcome to the custom page</h1>
{% endblock %}
```
> Note: Blocks not overridden by the child retain their parent layout default contents.

---

### Partials & Sub-Templates

Embed reusable components across templates:

```html
<div class="sidebar">
  {% include "partials/user_card" %}
  {% include "partials/recent_posts" %}
</div>
```

---

### Template Comments

Comments inside `{# ... #}` are stripped during tokenization and will **never** appear in the rendered HTML output:

```html
{# This is a developer note that won't appear in HTML output #}
<p>Visible content</p>
```

---

### Whitespace Control

Use leading/trailing hyphens to trim adjacent whitespace and newline characters:

```html
Hello   {{- user.name -}}   World
<!-- Output: HelloRiadWorld -->

<div>
  {%- if True -%}
    <span>Clean layout without empty lines</span>
  {%- endif -%}
</div>
```

---

## 🎨 Built-In Filter Catalog

| Filter | Parameters | Description | Example Input | Result |
|---|---|---|---|---|
| `upper` | — | Converts string to uppercase | `{{ "riad" \| upper }}` | `"RIAD"` |
| `lower` | — | Converts string to lowercase | `{{ "RIAD" \| lower }}` | `"riad"` |
| `capitalize` | — | Capitalizes the first character | `{{ "hello world" \| capitalize }}` | `"Hello world"` |
| `title` | — | Capitalizes each word in title case | `{{ "hello world" \| title }}` | `"Hello World"` |
| `trim` | — | Strips surrounding whitespace | `{{ "  hi  " \| trim }}` | `"hi"` |
| `ltrim` | — | Strips leading whitespace | `{{ "  hi" \| ltrim }}` | `"hi"` |
| `rtrim` | — | Strips trailing whitespace | `{{ "hi  " \| rtrim }}` | `"hi"` |
| `length` / `len` | — | Returns length of string, array, or map | `{{ [1,2,3] \| length }}` | `3` |
| `reverse` | — | Reverses a string or array | `{{ "abc" \| reverse }}` | `"cba"` |
| `first` | — | Returns first item of collection or char | `{{ ["a","b"] \| first }}` | `"a"` |
| `last` | — | Returns last item of collection or char | `{{ ["a","b"] \| last }}` | `"b"` |
| `join` | `delimiter = ", "` | Joins array elements with a separator | `{{ ["a","b"] \| join(" • ") }}` | `"a • b"` |
| `default` | `fallback` | Returns fallback if value is empty/null/false | `{{ "" \| default("N/A") }}` | `"N/A"` |
| `json` | — | Serializes object into JSON format | `{{{ user \| json }}}` | `{"name":"Ali"}` |
| `replace` | `old, new` | Replaces occurrences of substring | `{{ "a-b" \| replace("-", "/") }}`| `"a/b"` |
| `abs` | — | Returns absolute numerical value | `{{ -15 \| abs }}` | `15` |
| `round` | `digits = 0` | Rounds number to decimal precision | `{{ 3.14159 \| round(2) }}` | `3.14` |
| `int` | — | Casts value to integer | `{{ "42" \| int }}` | `42` |
| `float` | — | Casts value to floating-point number | `{{ "3.14" \| float }}` | `3.14` |
| `str` | — | Casts value to string | `{{ 123 \| str }}` | `"123"` |

---

## 🛠️ Custom Filter Registration

Extend Qalam by registering custom domain-specific filters:

```djazair
use qalam

let v = new qalam.View({})

# Register custom currency formatter
v.registerFilter("currency", fn(val, args = [])
    let code = if args.length() > 0 ? str(args[0]) else "USD"
    return str(val) + " " + code
end)

# Register custom badge generator
v.registerFilter("statusBadge", fn(status, args = [])
    if status == "active"
        return "<span class='badge-green'>ACTIVE</span>"
    end
    return "<span class='badge-red'>INACTIVE</span>"
end)

let html = v.renderString(
    "<p>Price: {{ amount | currency('DZD') }}</p><div>{{{ status | statusBadge }}}</div>",
    {"amount": 4500, "status": "active"}
)

print(html)
```

---

## 📚 API Reference

### Class: `qalam.View`

The primary template renderer orchestrator.

#### Constructor: `new qalam.View(config = {})`

**Configuration Map:**

| Key | Type | Default | Description |
|---|---|---|---|
| `"views"` | `String` | `"views"` | Base directory for template files |
| `"cache"` | `Boolean` | `True` | Enables in-memory compilation AST caching |

#### Methods

- **`v.render(templateName, data = {}) -> String`**
  Renders a template file by path identifier (e.g. `"home"` or `"trades/list"`). Supports layout inheritance and partials.

- **`v.renderString(templateString, data = {}, blocksMap = {}) -> String`**
  Renders an in-memory template string directly without accessing the filesystem.

- **`v.registerFilter(name, callback)`**
  Registers a custom filter function `fn(value, args = [])`.

- **`v.getSafePath(templateName) -> String`**
  Resolves and validates an absolute filesystem path, verifying directory boundary security.

- **`v.readFile(templateName) -> String`**
  Reads template file contents from disk.

- **`v.clearCache()`**
  Clears the in-memory compiled AST cache.

---

### Helper: `qalam.renderString()`

```djazair
qalam.renderString(templateString, data = {}, blocksMap = {}) -> String
```

Convenience function for quick template rendering without instantiating a View instance manually:

```djazair
let output = qalam.renderString("Hello, {{ name }}!", {"name": "Riad"})
```

---

### Security: `qalam.escapeHtml()`

```djazair
qalam.escapeHtml(value) -> String
```

Encodes HTML characters (`&`, `<`, `>`, `"`, `'`) to prevent XSS injection:

```djazair
let safe = qalam.escapeHtml("<script>alert('xss')</script>")
# → &lt;script&gt;alert(&#039;xss&#039;)&lt;/script&gt;
```

---

## 🛡️ Security Model

| Threat Vector | Mitigation Strategy in Qalam |
|---|---|
| **Cross-Site Scripting (XSS)** | All `{{ ... }}` output is auto-escaped via `escapeHtml()`. Raw output is strictly opt-in via `{{{ ... }}}`. |
| **Directory Traversal (`../`)** | `TemplateLoader.getSafePath()` strictly enforces directory containment boundaries. Path escape attempts throw fatal security errors. |
| **Null-Byte Injection** | Detected and rejected before path resolution. |
| **Information Leakage** | Descriptive template errors include line numbers without leaking internal server secrets. |

---

## ⚡ Performance & AST Caching

In production web applications, parsing templates on every request creates unnecessary CPU and memory overhead.

Qalam includes an intelligent **In-Memory AST Cache**:
1. When a template is first requested via `v.render("template")`, it is read, tokenized, and parsed into an AST tree.
2. The compiled AST is cached in memory.
3. Subsequent requests execute directly from the cached AST tree against the new request context data, achieving **up to 30x faster execution**.

To disable caching during local development, set `"cache": False`:
```djazair
let devView = new qalam.View({"views": "views", "cache": False})
```

---

## 🧪 Testing & Quality Assurance

Qalam includes a comprehensive automated test suite with **88 tests** covering all features:

```sh
# Run all test suites
djazair tests/run_all_tests.dz

# Or run individual test suites:
djazair tests/test_engine.dz
djazair tests/test_security.dz
djazair tests/test_conditions_loops.dz
djazair tests/test_filters.dz
djazair tests/test_inheritance.dz
```

---

## 📄 License & Author

- **Author:** Harizi Riyadh ([@hariziriyadh](https://github.com/hariziriyadh))
- **License:** [MIT License](./LICENSE)
- **Repository:** [djazair-extensions/qalam](https://github.com/djazair-language/djazair-extensions/tree/main/qalam)
