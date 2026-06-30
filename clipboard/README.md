# Djazair Clipboard Extension Manual

The **Clipboard** extension provides system clipboard read and write access for the Djazair programming language. Currently supports Windows (Win32 API).

---

## Table of Contents
1. [Installation](#1-installation)
2. [Quick Start](#2-quick-start)
3. [API Reference](#3-api-reference)
4. [Complete Examples](#4-complete-examples)

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

clipboard.setText("Hello from Djazair!")
print(clipboard.getText())

if clipboard.hasText()
    print("Clipboard contains text")
end

clipboard.clear()
```

---

## 3. API Reference

All functions are exported directly from the `clipboard` module.

### `getText()` → String
Returns the current clipboard text content. Returns empty string `""` if clipboard is empty or contains non-text data.

```dz
let text = clipboard.getText()
if text != ""
    print("Clipboard: " + text)
end
```

### `setText(text)` → Bool
Copies a string to the system clipboard. Returns `True` on success, `False` on failure.

```dz
let ok = clipboard.setText("Hello")
if ok
    print("Text copied!")
else
    print("Failed to copy")
end
```

### `clear()` → Bool
Empties the clipboard contents. Returns `True` on success.

```dz
clipboard.clear()
```

### `hasText()` → Bool
Returns `True` if the clipboard currently contains text data.

```dz
if clipboard.hasText()
    print("Clipboard has text: " + clipboard.getText())
end
```

---

## 4. Complete Examples

### 4.1. Basic Copy/Paste

```dz
use clipboard

clipboard.setText("Djazair Programming Language")
let value = clipboard.getText()
print("Read: '" + value + "'")

clipboard.clear()
print("After clear: '" + clipboard.getText() + "'")
```

### 4.2. Unicode and Multi-line

```dz
use clipboard

clipboard.setText("مرحبا بالعالم\nHello World\nこんにちは")
let text = clipboard.getText()
print(text)
```

All examples are available in the `examples/` directory.
