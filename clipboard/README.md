# Djazair Clipboard Extension Manual

The **Clipboard** extension provides system clipboard access for the Djazair programming language.
Cross-platform support: Windows (Win32 API), macOS (`pbcopy`/`pbpaste`), Linux (`xclip`).

---

## Table of Contents
1. [Installation](#1-installation)
2. [Quick Start](#2-quick-start)
3. [API Reference](#3-api-reference)
4. [Platform Notes](#4-platform-notes)
5. [Complete Examples](#5-complete-examples)

---

## 1. Installation

Build the native module:

```bash
cd extensions/clipboard
.\build.bat <path\to\djazair-language>
```

Then import it:

```djazair
use clipboard
```

---

## 2. Quick Start

```dz
use clipboard

clipboard.copy("Hello from Djazair!")
print(clipboard.paste())

if clipboard.has()
    print("Clipboard has text")
end

clipboard.clear()
```

---

## 3. API Reference

All functions are exported directly from the `clipboard` module.

### `paste()` → String
Returns the current clipboard text. Empty string if clipboard is empty or contains non-text data.

```dz
let text = clipboard.paste()
if text != ""
    print(text)
end
```

### `copy(text)` → Bool
Copies a string to the system clipboard. Returns `True` on success.

```dz
let ok = clipboard.copy("Hello")
if ok
    print("Copied!")
end
```

### `clear()` → Bool
Empties the clipboard. Returns `True` on success.

```dz
clipboard.clear()
```

### `has()` → Bool
Returns `True` if the clipboard contains text data.

```dz
if clipboard.has()
    print("Text available: " + clipboard.paste())
end
```

---

## 4. Platform Notes

| Platform | Implementation | Requirement |
|----------|---------------|-------------|
| Windows  | Win32 API (`user32.dll`) | None (built-in) |
| macOS    | `pbcopy` / `pbpaste` | None (built-in) |
| Linux    | `xclip` | `xclip` package |

On Linux, install `xclip` via your package manager:

```bash
# Debian / Ubuntu
sudo apt install xclip

# Fedora
sudo dnf install xclip

# Arch
sudo pacman -S xclip
```

---

## 5. Complete Examples

### 5.1. Basic Copy/Paste

```dz
use clipboard

clipboard.copy("Djazair Programming Language")
let value = clipboard.paste()
print("Paste: '" + value + "'")

clipboard.clear()
print("After clear: '" + clipboard.paste() + "'")
```

### 5.2. Unicode and Multi-line

```dz
use clipboard

clipboard.copy("مرحبا بالعالم\nHello World\nこんにちは")
let text = clipboard.paste()
print(text)
```

All examples are available in the `examples/` directory.
