# Qt Extension for Djazair

<div align="center">

[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)
[![Organization](https://img.shields.io/badge/organization-djazair--language-blue.svg)](https://github.com/djazair-language)
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg)](https://github.com/djazair-language/djazair-extensions/pulls)

**Official Qt 5 / Qt 6 cross-platform GUI framework extension for the Djazair Programming Language.**

</div>

---

## ⚡ Installation

Install the package using **DPM (Djazair Package Manager)**:

```bash
dpm install qt
```

---

## 🛠️ Building Native Extension

Build the native C++ dynamic library (`qt.dll` / `qt.so`):

```bash
dpm build qt
```

> **Requirements**: MinGW-w64 with Qt5 / Qt6 on Windows, or GCC with `pkg-config Qt5Widgets` on Linux/macOS.

---

## 🚀 Quick Example

```djazair
import "qt" as qt

let app = new qt.Application()

let window = new qt.MainWindow()
window.setTitle("Hello Djazair Qt")
window.resize(600, 400)

let layout = new qt.VBoxLayout()

let label = new qt.Label("Native GUI with Djazair & Qt!")
label.setStyleSheet("font-size: 18px; color: #2980b9; font-weight: bold;")

let btn = new qt.Button("Click Me")
btn.setStyleSheet("background-color: #27ae60; color: white; padding: 8px; border-radius: 4px;")

layout.addWidget(label)
layout.addWidget(btn)

let container = new qt.Widget()
container.setLayout(layout)

window.setCentralWidget(container)
window.show()

app.exec()
```

---

## 📖 API Reference

### Application & Lifecycle
- `Application()`: Initializes Qt application instance.
- `app.exec()`: Starts the main event loop.
- `app.processEvents()`: Processes pending events manually.
- `app.quit()`: Exits application execution.

### Components & Widgets
- `Widget(parent = null)`: Base widget component.
- `Button(text, parent = null)`: Push button component.
- `Label(text, parent = null)`: Text label component.
- `LineEdit(text, parent = null)`: Single-line input field.
- `TextEdit(text, parent = null)`: Multi-line rich/plain text editor.
- `ListWidget(parent = null)`: Items list view component.
- `TableWidget(rows, cols, parent = null)`: Grid data table component.

### Layout Managers
- `VBoxLayout()`: Vertical box layout manager.
- `HBoxLayout()`: Horizontal box layout manager.
- `GridLayout()`: Grid layout manager.

### Windows & Dialogs
- `MainWindow()`: Top-level desktop main window.
- `MessageBox.info(title, message)`: Informational alert modal.
- `FileDialog.getOpenFileName(title, filter)`: File picker dialog.
- `FileDialog.getSaveFileName(title, filter)`: File saver dialog.

---

## 📄 License

Licensed under the MIT License.
