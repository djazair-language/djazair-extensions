# Djazair Qt Framework Extension (`qt`)

Enterprise GUI, Multimedia, Dynamic UI Designer (.ui) Loader, and System Integration extension for the **Djazair Programming Language**.

---

## 🌟 Key Features

- **Modern Desktop Widgets**: Buttons, Labels, LineEdits, TextEdits, CheckBoxes, RadioButtons, ComboBoxes, ProgressBars, Sliders, SpinBoxes.
- **Dynamic Qt Designer Loader (`qt.loadUi`)**: Dynamically parse `.ui` XML layout files created with Qt Designer at runtime.
- **Multimedia Engine (`qt.MediaPlayer` & `qt.VideoWidget`)**: Native video and audio playback directly inside Djazair applications.
- **Complex Data Views**: Multi-column `TreeWidget` (with `TreeItem` wrappers), `TableWidget`, and `ListWidget`.
- **Flexible Layout Managers**: `VBoxLayout`, `HBoxLayout`, `GridLayout`, and `FormLayout` with `setMargins()` & `setSpacing()`.
- **System Integration**: Modals (`MessageBox` with info/warning/error/question), `FileDialog`, `InputDialog`, `QTimer`, and `SystemTrayIcon`.
- **Preset Themes**: Instant modern styling via `Theme.applyDark()` and `Theme.applyLight()`.

---

## 🚀 Quick Start Example

```djazair
use qt

let app = new qt.Application()

let window = new qt.MainWindow()
window.setWindowTitle("Djazair Qt Quick Start")
window.resize(600, 400)

let label = new qt.Label("Hello from Djazair Qt!")
let button = new qt.Button("Click Me")

let vbox = new qt.VBoxLayout()
vbox.setMargins(15, 15, 15, 15)
vbox.setSpacing(10)
vbox.addWidget(label)
vbox.addWidget(button)

let central = new qt.Widget()
central.setLayout(vbox)

let theme = new qt.Theme()
theme.applyDark(window)

window.setCentralWidget(central)
window.show()

app.exec()
```

---

## 📚 API Reference Overview

### Core & Application
- `new qt.Application()`
- `app.exec()`
- `new qt.Timer()`: `.start(msec)`, `.stop()`
- `qt.loadUi(filepath)`: Load `.ui` XML file dynamically.

### Widgets & Controls
- `new qt.Widget(parent)`: `.show()`, `.hide()`, `.setVisible(bool)`, `.setWindowTitle(title)`, `.resize(w, h)`, `.setLayout(layout)`, `.setStyleSheet(css)`, `.setToolTip(text)`
- `new qt.Button(text, parent)`: `.setText(text)`
- `new qt.Label(text, parent)`: `.setText(text)`, `.getText()`
- `new qt.LineEdit(text, parent)`: `.setText(text)`, `.getText()`, `.setPlaceholder(text)`, `.setEchoMode(mode)`
- `new qt.TextEdit(text, parent)`: `.setText(text)`, `.getText()`, `.clear()`, `.append(text)`
- `new qt.CheckBox(text, parent)`: `.isChecked()`
- `new qt.ComboBox(parent)`: `.addItem(text)`, `.getCurrentText()`
- `new qt.ProgressBar(parent)`: `.setRange(min, max)`, `.setValue(val)`, `.getValue()`
- `new qt.Slider(orient, parent)`: `.setRange(min, max)`, `.setValue(val)`, `.getValue()`
- `new qt.SpinBox(parent)`: `.setRange(min, max)`, `.getValue()`

### Containers & Views
- `new qt.TabWidget(parent)`: `.addTab(widget, title)`
- `new qt.StackedWidget(parent)`: `.addWidget(widget)`
- `new qt.GroupBox(title, parent)`
- `new qt.ScrollArea(parent)`: `.setWidget(widget)`
- `new qt.Splitter(orient, parent)`: `.addWidget(widget)`
- `new qt.ListWidget(parent)`: `.addItem(text)`
- `new qt.TreeWidget(parent)`: `.setHeaders(headersArray)`, `.addItem(colsArray, parentItem)` -> returns `TreeItem`
- `new qt.TableWidget(rows, cols, parent)`: `.setItem(row, col, text)`

### Layout Managers
- `new qt.VBoxLayout(parent)`: `.addWidget(widget)`, `.setMargins(l, t, r, b)`, `.setSpacing(s)`
- `new qt.HBoxLayout(parent)`: `.addWidget(widget)`, `.setMargins(l, t, r, b)`, `.setSpacing(s)`
- `new qt.GridLayout(parent)`: `.addWidget(widget, row, col, rowSpan, colSpan)`, `.setMargins(l, t, r, b)`, `.setSpacing(s)`
- `new qt.FormLayout(parent)`: `.addRow(label, fieldWidget)`, `.setMargins(l, t, r, b)`, `.setSpacing(s)`

### Windows & Dialogs
- `new qt.MainWindow()`: `.setCentralWidget(widget)`, `.setWindowTitle(title)`, `.addMenu(title)` -> returns `Menu`, `.addToolBar(title)`, `.setStatusMessage(msg, timeout)`
- `Menu`: `.addMenu(title)`, `.addAction(title)` -> returns `Action`, `.addSeparator()`
- `qt.MessageBox`: `.info(title, msg)`, `.warning(title, msg)`, `.error(title, msg)`, `.question(title, msg)` (returns `bool`)
- `qt.FileDialog`: `.getOpenFileName()`, `.getSaveFileName()`, `.getExistingDirectory()`
- `qt.InputDialog`: `.getText(title, label)`
- `new qt.SystemTrayIcon(tooltip)`: `.show()`, `.setContextMenu(menu)`, `.showMessage(title, msg, iconType, timeout)`

### Multimedia Engine
- `new qt.MediaPlayer()`: `.setMedia(pathOrUrl)`, `.play()`, `.pause()`, `.stop()`, `.setVideoOutput(videoWidget)`
- `new qt.VideoWidget(parent)`

### Themes
- `new qt.Theme()`: `.applyDark(widget)`, `.applyLight(widget)`
