# Djazair Clipboard Extension Manual

<div align="center">

[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)
[![Djazair: v0.2.0](https://img.shields.io/badge/version-v0.2.0-blue.svg)](dpm.json)

**Production-grade system clipboard integration for the Djazair Programming Language.**

</div>

---

The **Clipboard** extension provides seamless, cross-platform system clipboard access for Djazair applications. It features high resilience against transient OS locks, full Unicode (UTF-8) support, $O(1)$ change detection via sequence numbers, and format introspection.

---

## 📋 Table of Contents
1. [Key Features](#-key-features)
2. [Installation & Build](#-installation--build)
3. [Quick Start](#-quick-start)
4. [API Reference](#-api-reference)
5. [Platform Architecture](#-platform-architecture)
6. [Examples](#-examples)

---

## 🚀 Key Features

- **High-Resilience Win32 Engine**: Automatic retry loop with backoff prevents failures caused by external clipboard locks (Office, browsers, clipboard managers).
- **Binary-Safe Unicode UTF-8**: Full multilingual support including Arabic, East Asian scripts, emojis, and symbols.
- **$O(1)$ Change Tracking**: `sequenceNumber()` enables instantaneous change detection without costly data reads.
- **Format Introspection**: `formats()` inspects available data formats (Unicode, ANSI text, Bitmaps, Files, etc.).
- **Defensive Type Safety**: Strict type assertions using standard `assert` module.
- **Cross-Platform**: Native Win32 API on Windows, Wayland (`wl-clipboard`) & X11 (`xclip`/`xsel`) on Linux, and `pbcopy`/`pbpaste` on macOS.

---

## 🛠️ Installation & Build

### Build Native DLL / Shared Object

```bash
# Windows
.\build.bat <path\to\djazair-language>

# Linux / macOS
./build.sh <path/to/djazair-language>
```

### Import into Djazair Script

```dz
use clipboard
```

---

## 💡 Quick Start

```dz
use clipboard

# Copy Unicode text to clipboard
clipboard.copy("مرحبا بالعالم — Hello from Djazair 🇩🇿")

# Paste current content
print(clipboard.paste())

# Check availability and clear
if clipboard.has()
    print("Clipboard contains text")
end

clipboard.clear()
```

---

## 📖 API Reference

### Core Operations

| Function | Arguments | Returns | Description |
| :--- | :--- | :--- | :--- |
| `copy(text)` | `text: String` | `Bool` | Copies UTF-8 string to clipboard. Throws `TypeError` if input is non-string. |
| `paste()` | *None* | `String` | Retrieves current text content. Returns `""` if empty or non-text. |
| `clear()` | *None* | `Bool` | Clears all data from the system clipboard. |
| `has()` | *None* | `Bool` | Checks whether the clipboard currently contains text data. |

### Introspection & Tracking

| Function | Arguments | Returns | Description |
| :--- | :--- | :--- | :--- |
| `sequenceNumber()` | *None* | `Number` | Returns a serial counter incremented on every clipboard change ($O(1)$ check). |
| `formats()` | *None* | `Array` | Returns array of available format names (e.g., `["unicode", "text", "bitmap", "files"]`). |

### Standard Semantic Aliases

| Alias Function | Equivalent To | Description |
| :--- | :--- | :--- |
| `setText(text)` | `copy(text)` | Semantic alias conforming to UI / Widget standards. |
| `getText()` | `paste()` | Semantic alias conforming to UI / Widget standards. |
| `hasText()` | `has()` | Semantic alias conforming to UI / Widget standards. |

---

## 🖥️ Platform Architecture

| Platform | Implementation | Requirements |
| :--- | :--- | :--- |
| **Windows** | Direct Win32 API (`user32.dll`) | Built-in (Windows 7 / 10 / 11 / Server) |
| **Linux (Wayland)** | Native `wl-clipboard` pipe (`wl-copy`/`wl-paste`) | `wl-clipboard` package |
| **Linux (X11)** | Auto-detected `xclip` or `xsel` pipe | `xclip` or `xsel` package |
| **macOS** | Native `pbcopy` / `pbpaste` pipe | Built-in |

### Linux Package Installation

```bash
# Ubuntu / Debian
sudo apt install xclip wl-clipboard

# Fedora
sudo dnf install xclip wl-clipboard

# Arch Linux
sudo pacman -S xclip wl-clipboard
```

---

## 📂 Examples

### 1. Change Detection Loop ($O(1)$ Tracking)

```dz
use clipboard
use os

let lastSeq = clipboard.sequenceNumber()
print("Monitoring clipboard changes (current seq: ${lastSeq})...")

# Simulate a check after external modification
clipboard.setText("New copied item")
let currentSeq = clipboard.sequenceNumber()

if currentSeq > lastSeq
    print("Detected clipboard update: '" + clipboard.getText() + "'")
end
```

### 2. Inspecting Available Formats

```dz
use clipboard

clipboard.setText("Inspect formats test")
let formatsList = clipboard.formats()

print("Available Formats:")
for fmt in formatsList
    print("  - " + fmt)
end
```

### 3. Full Unicode and Multiline

```dz
use clipboard

let payload = "Algeria (الجزائر)\nCapital: Algiers\nStatus: 🇩🇿 Online"
clipboard.setText(payload)

print(clipboard.getText())
```

All examples are executable in the `examples/` directory.

---

## 📄 License

This extension is licensed under the MIT License.

