# Djazair Qt Framework Extension (`qt`)

Enterprise-grade GUI, Multimedia, 2D Drawing, Charts, and System Integration extension for the **Djazair Programming Language**. 

Built on top of C++ Qt, this extension provides a native, highly optimized, and modern API to build cross-platform desktop applications in Djazair. It strictly adheres to the standard Qt `camelCase` naming conventions and `Q`-prefixed class names (e.g., `QMainWindow`, `QPushButton`) to provide a familiar experience for both beginners and seasoned Qt professionals.

---

## 🌟 Key Features

- **Modern Desktop Widgets**: `QPushButton`, `QLabel`, `QLineEdit`, `QTextEdit`, `QPlainTextEdit`, `QCheckBox`, `QRadioButton`, `QComboBox`, `QProgressBar`, `QSlider`, `QSpinBox`, `QDoubleSpinBox`.
- **Flexible Layout Managers**: `QVBoxLayout`, `QHBoxLayout`, `QGridLayout`, and `QFormLayout` with full margins and spacing control.
- **Complex Data Views**: Multi-column `QTreeWidget` (with `QTreeWidgetItem`), `QTableWidget`, and `QListWidget`.
- **Advanced 2D Drawing (Canvas)**: Double-buffered `QCanvas` with hardware-accelerated `QPainter` (lines, rects, ellipses, polygons) and `QPixmap` (image loading/saving).
- **Interactive Data Visualization (Charts)**: `QChart`, `QChartView`, `QLineSeries`, `QPieSeries`, `QBarSeries`, `QBarSet`, and `QScatterSeries` with customizable axes.
- **Drag & Drop**: Native drag-and-drop support across all widgets (`setAcceptDrops`, `onDrop`).
- **Dynamic UI Loader (`qt.loadUi`)**: Dynamically parse `.ui` XML layout files created with Qt Designer at runtime.
- **Multimedia Engine (`QMediaPlayer` & `QVideoWidget`)**: Native video and audio playback directly inside Djazair applications.
- **System Integration**: Modals (`QMessageBox`, `QFileDialog`, `QInputDialog`), background timers (`QTimer`), and system tray (`QSystemTrayIcon`).
- **Preset Themes**: Instant modern styling via `Theme.applyDark()` and `Theme.applyLight()`.

---

## 🚀 Quick Start Example

A minimal example to create a window with a button and a layout:

```djazair
use qt

# 1. Initialize the application
let app = new qt.QApplication()

# 2. Create the main window
let mainWindow = new qt.QMainWindow()
mainWindow.setWindowTitle("Djazair Qt Quick Start")
mainWindow.resize(600, 400)

# 3. Create widgets
let label = new qt.QLabel("Hello from Djazair Qt!")
let button = new qt.QPushButton("Click Me")

# 4. Arrange widgets in a Vertical Box Layout
let vbox = new qt.QVBoxLayout()
vbox.setMargins(15, 15, 15, 15)
vbox.setSpacing(10)
vbox.addWidget(label)
vbox.addWidget(button)

# 5. Set the central widget
let central = new qt.QWidget()
central.setLayout(vbox)
mainWindow.setCentralWidget(central)

# 6. Apply a professional dark theme and show
let theme = new qt.Theme()
theme.applyDark(mainWindow)
mainWindow.show()

# 7. Start the event loop
app.exec()
```

---

## Build and verification

The native bridge requires the Qt Widgets, UiTools, Multimedia,
MultimediaWidgets, and Charts development modules. The build scripts resolve
their source files relative to the extension directory, so they may be run
from any working directory.

```bat
extensions\qt\build.bat <path-to-djazair-language>
set QT_QPA_PLATFORM=offscreen
build\bin\djazair.exe extensions\qt\tests\smoke.dz
```

`tests/smoke.dz` is non-interactive and covers module loading, tree/table
views, painting, canvas ownership, timers, and synchronous widget callbacks.
Use `QT_QPA_PLATFORM=offscreen` only for headless testing; normal desktop
applications should use their system Qt platform plugin.

### Resource ownership

Qt parents own their child objects. The bridge tracks QObject destruction, so
a Djazair handle whose parent has already deleted it fails safely instead of
dereferencing stale memory. After calling `destroy()` or `endPaint()` on a
manual resource, do not reuse that object. For `QCanvas`, finish a drawing
session with `canvas.endPaint()`; calling `painter.endPaint()` first is safe
and only ends the active paint operation.

---

## 📚 API Reference Overview

### 1. Core & Application
- `new qt.QApplication()`: Initializes the Qt GUI subsystem.
- `app.exec()`: Starts the main event loop.
- `new qt.QTimer()`: `.start(msec)`, `.stop()`.
- `qt.loadUi(filepath)`: Loads a `.ui` XML file dynamically and returns the widget handle.

### 2. Base Widget Methods
All GUI elements inherit from `QWidget` and share these methods:
- `.show()`, `.hide()`, `.setVisible(bool)`
- `.setWindowTitle(title)`, `.resize(w, h)`, `.setGeometry(x, y, w, h)`
- `.setLayout(layout)`, `.setStyleSheet(css)`, `.setToolTip(text)`, `.setEnabled(bool)`
- **Drag & Drop**: `.setAcceptDrops(bool)`, `.onDrop(fn(text) { ... })`

### 3. Controls & Inputs
- `new qt.QPushButton(text, parent)`: `.setText(text)`
- `new qt.QLabel(text, parent)`: `.setText(text)`, `.getText()`, `.setAlignment(alignInt)`
- `new qt.QLineEdit(text, parent)`: `.setText(text)`, `.getText()`, `.setPlaceholder(text)`, `.setEchoMode(mode)`
- `new qt.QTextEdit(text, parent)` / `QPlainTextEdit(text, parent)`: `.setText(text)`, `.getText()`, `.clear()`, `.append(text)`
- `new qt.QCheckBox(text, parent)`: `.isChecked()`
- `new qt.QRadioButton(text, parent)`: `.isChecked()`
- `new qt.QComboBox(parent)`: `.addItem(text)`, `.getCurrentText()`
- `new qt.QProgressBar(parent)`: `.setRange(min, max)`, `.setValue(val)`, `.getValue()`
- `new qt.QSlider(orient, parent)`: `.setRange(min, max)`, `.setValue(val)`, `.getValue()`
- `new qt.QSpinBox(parent)` / `QDoubleSpinBox(parent)`: `.setRange(min, max)`, `.setValue(val)`, `.getValue()`

### 4. Containers & Views
- `new qt.QTabWidget(parent)`: `.addTab(widget, title)`
- `new qt.QStackedWidget(parent)`: `.addWidget(widget)`
- `new qt.QGroupBox(title, parent)`
- `new qt.QScrollArea(parent)`: `.setWidget(widget)`
- `new qt.QSplitter(orient, parent)`: `.addWidget(widget)`
- `new qt.QListWidget(parent)`: `.addItem(text)`
- `new qt.QTreeWidget(parent)`: `.setHeaders(headersArray)`, `.addItem(colsArray, parentItem)` -> returns `QTreeWidgetItem`
- `new qt.QTableWidget(rows, cols, parent)`: `.setItem(row, col, text)`

### 5. Layout Managers
- `new qt.QVBoxLayout(parent)`: `.addWidget(widget)`, `.setMargins(l, t, r, b)`, `.setSpacing(s)`
- `new qt.QHBoxLayout(parent)`: `.addWidget(widget)`, `.setMargins(l, t, r, b)`, `.setSpacing(s)`
- `new qt.QGridLayout(parent)`: `.addWidget(widget, row, col, rowSpan, colSpan)`, `.setMargins(l, t, r, b)`, `.setSpacing(s)`
- `new qt.QFormLayout(parent)`: `.addRow(label, fieldWidget)`, `.setMargins(l, t, r, b)`, `.setSpacing(s)`

### 6. Windows, Menus & Dialogs
- `new qt.QMainWindow()`: `.setCentralWidget(widget)`, `.setWindowTitle(title)`, `.addMenu(title)` -> returns `QMenu`, `.addToolBar(title)` -> returns `QToolBar`, `.setStatusMessage(msg, timeout)`
- `QMenu`: `.addMenu(title)`, `.addAction(title)` -> returns `QAction`, `.addSeparator()`
- `new qt.QDialog(parent)`: `.exec()`
- `qt.QMessageBox`: `.info(title, msg)`, `.warning(title, msg)`, `.error(title, msg)`, `.question(title, msg)` (returns boolean)
- `qt.QFileDialog`: `.getOpenFileName()`, `.getSaveFileName()`, `.getExistingDirectory()`
- `qt.QInputDialog`: `.getText(title, label)`
- `new qt.QSystemTrayIcon(tooltip)`: `.show()`, `.setContextMenu(menu)`, `.showMessage(title, msg, iconType, timeout)`

### 7. 2D Drawing & Canvas (`QCanvas`, `QPainter`, `QPixmap`)
- `new qt.QPixmap(width, height)`: `.fill(hex_color)`, `.save(path)`. Load via `qt.pixmapLoad(path)`
- `new qt.QCanvas(parent)`: A double-buffered drawing widget.
  - `.begin()`: Starts drawing, returns a `QPainter` instance.
  - `.endPaint()`: Finishes drawing and updates the screen.
  - `.clear(hex_color)`: Clears the canvas.
- `QPainter` methods: 
  - `.setPen(hex_color, width)`, `.setPenStyle(style, width, color)`, `.setBrush(hex_color)`, `.setFont(family, size, bold, italic)`
  - `.drawLine(x1, y1, x2, y2)`, `.drawRect(x, y, w, h)`, `.drawRoundedRect(x, y, w, h, radius)`, `.drawEllipse(x, y, w, h)`
  - `.drawText(x, y, text)`, `.drawPixmap(x, y, pixmap)`

### 8. Data Visualization (`QChart`)
- `new qt.QChart()`: `.setTitle(title)`, `.addSeries(series)`, `.createDefaultAxes()`, `.setAxisTitle(orient, title)`, `.setAxisRange(orient, min, max)`
- `new qt.QChartView(parent)`: `.setChart(chart)`
- **Series Types**:
  - `new qt.QLineSeries()`: `.append(x, y)`
  - `new qt.QScatterSeries()`: `.append(x, y)`
  - `new qt.QPieSeries()`: `.append(label, value)`
  - `new qt.QBarSeries()`: `.append(barSet)`. (`new qt.QBarSet(label)` -> `.append(value)`)

### 9. Multimedia Engine
- `new qt.QMediaPlayer()`: `.setMedia(pathOrUrl)`, `.play()`, `.pause()`, `.stop()`, `.setVideoOutput(videoWidget)`
- `new qt.QVideoWidget(parent)`: A widget to render the video frames.

### 10. Themes
- `new qt.Theme()`: Built-in professional themes.
  - `.applyDark(widget)`
  - `.applyLight(widget)`

---

## 🛠 Advanced Features

### Handling Drag & Drop
Enable drag & drop on any widget and bind a callback using `onDrop`:
```djazair
let dropZone = new qt.QLabel("Drop files here")
dropZone.setAcceptDrops(True)
dropZone.onDrop(fn(data) {
    qt.QMessageBox.info("File Dropped", "You dropped: " ~ data)
})
```

### Drawing with QCanvas
```djazair
let canvas = new qt.QCanvas()
let painter = canvas.begin()
if painter != Null
    painter.setPen("#ff0000", 2)
    painter.drawRect(50, 50, 200, 150)
    painter.endPaint()
end
canvas.endPaint()
```

### Creating Charts
```djazair
let chart = new qt.QChart()
chart.setTitle("Sales Growth")

let series = new qt.QLineSeries()
series.append(0, 10)
series.append(1, 45)
series.append(2, 25)

chart.addSeries(series)
chart.createDefaultAxes()

let view = new qt.QChartView()
view.setChart(chart)
```

---
*Developed for the Djazair Programming Language.*
