# Qalam — HTML Template Engine for Djazair

> **قلم** *(noun, Arabic)* — pen; the instrument of writing and expression.

A fast, secure, AST-based HTML template engine for the [Djazair programming language](https://github.com/djazair-language).
Works standalone — no web framework required.

[![Version](https://img.shields.io/badge/version-0.1.0-blue)](./dpm.json)
[![License](https://img.shields.io/badge/license-MIT-green)](./LICENSE)

---

## Features

- **Variable interpolation** — `{{ var }}` with automatic XSS escaping
- **Raw HTML output** — `{{{ raw }}}` for trusted content
- **Conditionals** — `{% if %} / {% else %} / {% endif %}`
- **Loops** — `{% for item in list %} / {% endfor %}`
- **Template inheritance** — `{% extends "layout" %}` + `{% block name %}`
- **Partials** — `{% include "partials/nav" %}`
- **Dot-notation** — `{{ user.profile.name }}` for nested data
- **In-memory rendering** — `renderString()` without any files on disk
- **XSS-safe by default** — all `{{ }}` output is HTML-escaped
- **Path traversal protection** — cannot escape the configured views directory

---

## Installation

```sh
dpm install qalam
```

---

## Quick Start

### In-memory rendering (no files needed)

```djazair
use qalam

let html = qalam.renderString(
    "<h1>Hello, {{ user.name }}!</h1>",
    {"user": {"name": "Riad"}}
)
print(html)
# → <h1>Hello, Riad!</h1>
```

### File-based rendering

```djazair
use qalam

let v = new qalam.view({"views": "views"})
let html = v.render("home", {"title": "Welcome"})
```

---

## Template Syntax

| Syntax | Description |
|---|---|
| `{{ var }}` | Output — **auto HTML-escaped** (XSS safe) |
| `{{{ raw }}}` | Raw unescaped HTML |
| `{% if cond %}` … `{% else %}` … `{% endif %}` | Conditional block |
| `{% for x in list %}` … `{% endfor %}` | Loop block |
| `{% extends "layout" %}` | Inherit from a parent layout |
| `{% block name %}` … `{% endblock %}` | Declare / override a content block |
| `{% include "partials/nav" %}` | Embed a sub-template |

---

## Variable Output

```html
<!-- Auto-escaped: safe against XSS -->
<h1>Hello, {{ user.name }}</h1>

<!-- Unescaped: use only for trusted HTML -->
<div>{{{ article.bodyHtml }}}</div>
```

Supports **dot-notation** for nested data and **negation** with `!`:

```html
{% if !user.isGuest %}
    <a href="/logout">Sign out</a>
{% endif %}
```

### Truthy / Falsy Rules

| Value | Truthy? |
|---|---|
| `True`, non-zero number, non-empty string | ✅ Yes |
| `False`, `0`, `""`, `Null`, `[]` | ❌ No |

---

## Conditionals

```html
{% if trade.pnl > 0 %}
    <span class="green">Profit</span>
{% else %}
    <span class="red">Loss</span>
{% endif %}
```

---

## Loops

```html
<ul>
{% for item in trades %}
    <li>{{ item.symbol }} — ${{ item.price }}</li>
{% endfor %}
</ul>
```

---

## Template Inheritance

**Base layout** (`views/layout.html`):
```html
<!DOCTYPE html>
<html>
<head>
    <title>{% block title %}My App{% endblock %}</title>
    <link rel="stylesheet" href="/style.css">
</head>
<body>
    <nav>{% include "partials/nav" %}</nav>
    <main>
        {% block content %}{% endblock %}
    </main>
    <footer>{% block footer %}&copy; 2026{% endblock %}</footer>
</body>
</html>
```

**Child template** (`views/trades/list.html`):
```html
{% extends "layout" %}

{% block title %}Trade List{% endblock %}

{% block content %}
<h1>Open Trades</h1>
<ul>
{% for trade in trades %}
    <li>{{ trade.symbol }} — ${{ trade.price }}</li>
{% endfor %}
</ul>
{% endblock %}
```

**Render from code:**
```djazair
use qalam

let v = new qalam.view({"views": "views"})
let html = v.render("trades/list", {"trades": tradeList})
```

> Blocks omitted in the child retain the layout's default content.

---

## Partials / Sub-templates

**Create a partial** (`views/partials/nav.html`):
```html
<ul class="nav">
    <li><a href="/">Home</a></li>
    <li><a href="/trades">Trades</a></li>
</ul>
```

**Include it anywhere:**
```html
{% include "partials/nav" %}
```

---

## In-memory Rendering

Render a template string without any files on disk:

```djazair
use qalam

# Module-level helper
let html = qalam.renderString(
    "Hello {{ user.name }}, your code is {{ code }}.",
    {"user": {"name": "Riad"}, "code": "XK-4921"}
)

# Or create a view instance directly
let v = new qalam.view({})
let snippet = v.renderString("<li>{{ item }}</li>", {"item": "First"})
```

**Use cases:** email bodies, generated code, CLI output, API responses.

---

## API Reference

### `qalam.view`

The main class for file-based rendering.

```djazair
let v = new qalam.view(config)
```

**Configuration:**

| Key | Type | Default | Description |
|---|---|---|---|
| `"views"` | String | `"views"` | Base directory for template files |

**Methods:**

| Method | Description |
|---|---|
| `v.render(name, data)` | Render a template file with data context |
| `v.renderString(tpl, data)` | Render a template string in memory |
| `v.getSafePath(name)` | Resolve and validate a template file path |
| `v.readFile(name)` | Read a template file from disk |

---

### `qalam.renderString(tpl, data)`

Convenience module-level function. Equivalent to `new qalam.view({}).renderString(tpl, data)`.

```djazair
let html = qalam.renderString("Hello {{ name }}!", {"name": "World"})
```

---

### `qalam.escapeHtml(str)`

Escapes HTML special characters: `&`, `<`, `>`, `"`, `'`.

```djazair
let safe = qalam.escapeHtml("<script>alert(1)</script>")
# → &lt;script&gt;alert(1)&lt;/script&gt;
```

---

## Security

| Threat | Protection |
|---|---|
| **XSS** | All `{{ }}` output is auto-escaped via `escapeHtml()` |
| **Raw HTML** | Only via explicit `{{{ }}}` — opt-in |
| **Path traversal** | `getSafePath()` enforces the configured `views` directory boundary |

---

## Using with Kasbah

Qalam is the built-in template engine for the [Kasbah](../kasbah) web framework.
When using Kasbah, `res.view()` delegates to Qalam automatically — no separate setup needed.

```djazair
use kasbah

let app = new kasbah.app({"views": "views"})
app.get("/", fn(req, res)
    res.view("home", {"title": "Welcome"})  # powered by qalam internally
end)
app.listen()
```

---

## Examples

See the [`examples/`](./examples/) directory:

| File | Topic |
|---|---|
| [`01_basic.dz`](./examples/01_basic.dz) | Variables, escaping, dot-notation |
| [`02_conditions.dz`](./examples/02_conditions.dz) | Conditionals |
| [`03_loops.dz`](./examples/03_loops.dz) | Loops and nested objects |
| [`04_inheritance.dz`](./examples/04_inheritance.dz) | Layout inheritance |
| [`05_partials.dz`](./examples/05_partials.dz) | Sub-template includes |
| [`06_string_render.dz`](./examples/06_string_render.dz) | In-memory rendering |

---

## Tests

```sh
djazair tests/test_engine.dz
djazair tests/test_security.dz
```

---

## License

MIT © Harizi Riyadh
