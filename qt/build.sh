#!/usr/bin/env bash
# ─────────────────────────────────────────────
#  build.sh  –  qt extension for Djazair (Linux / macOS)
#  Usage: ./build.sh <ROOT>
# ─────────────────────────────────────────────

set -e

if [ -z "$1" ]; then
    echo "[ERROR] Missing argument: ROOT path to djazair-language."
    echo "Usage: ./build.sh <path/to/djazair-language>"
    exit 1
fi

ROOT="$1"

if [ ! -d "$ROOT/src/include" ]; then
    echo "[ERROR] '$ROOT/src/include' not found. Is ROOT correct?"
    exit 1
fi

QT_CFLAGS=""
QT_LIBS=""

if pkg-config --exists Qt5Widgets; then
    QT_CFLAGS=$(pkg-config --cflags Qt5Widgets Qt5Core Qt5Gui)
    QT_LIBS=$(pkg-config --libs Qt5Widgets Qt5Core Qt5Gui)
elif pkg-config --exists Qt6Widgets; then
    QT_CFLAGS=$(pkg-config --cflags Qt6Widgets Qt6Core Qt6Gui)
    QT_LIBS=$(pkg-config --libs Qt6Widgets Qt6Core Qt6Gui)
else
    echo "[WARNING] pkg-config for Qt5/Qt6 not found. Attempting default fallback flags."
    QT_CFLAGS="-I/usr/include/qt -I/usr/include/qt/QtWidgets"
    QT_LIBS="-lQt5Widgets -lQt5Core -lQt5Gui"
fi

OUTPUT_LIB="qt.so"
if [[ "$OSTYPE" == "darwin"* ]]; then
    OUTPUT_LIB="qt.dylib"
fi

echo "[INFO] Building Qt extension..."

g++ -shared -fPIC -O2 -std=c++17 \
    -I"$ROOT/src/include" \
    -I"$ROOT/src/core" \
    -I"$ROOT/src/libs" \
    $QT_CFLAGS \
    src/qt_wrapper.cpp src/qt_djazair.cpp \
    -o "$OUTPUT_LIB" \
    -L"$ROOT/build/bin" -ldjazair $QT_LIBS

echo "[OK] $OUTPUT_LIB built successfully."
