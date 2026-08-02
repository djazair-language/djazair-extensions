#!/usr/bin/env bash
# ─────────────────────────────────────────────
#  build.sh  –  qt extension for Djazair (Linux / macOS)
#  Usage: ./build.sh <ROOT>
# ─────────────────────────────────────────────

set -eu

if [ -z "$1" ]; then
    echo "[ERROR] Missing argument: ROOT path to djazair-language."
    echo "Usage: ./build.sh <path/to/djazair-language>"
    exit 1
fi

ROOT=$(cd "$1" && pwd)
SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)

if [ ! -d "$ROOT/src/include" ]; then
    echo "[ERROR] '$ROOT/src/include' not found. Is ROOT correct?"
    exit 1
fi

QT_CFLAGS=""
QT_LIBS=""

if pkg-config --exists Qt5Widgets Qt5UiTools Qt5Multimedia Qt5MultimediaWidgets Qt5Charts; then
    QT_CFLAGS=$(pkg-config --cflags Qt5Widgets Qt5UiTools Qt5Multimedia Qt5MultimediaWidgets Qt5Charts)
    QT_LIBS=$(pkg-config --libs Qt5Widgets Qt5UiTools Qt5Multimedia Qt5MultimediaWidgets Qt5Charts)
elif pkg-config --exists Qt6Widgets Qt6UiTools Qt6Multimedia Qt6MultimediaWidgets Qt6Charts; then
    QT_CFLAGS=$(pkg-config --cflags Qt6Widgets Qt6UiTools Qt6Multimedia Qt6MultimediaWidgets Qt6Charts)
    QT_LIBS=$(pkg-config --libs Qt6Widgets Qt6UiTools Qt6Multimedia Qt6MultimediaWidgets Qt6Charts)
else
    echo "[ERROR] Qt Widgets, UiTools, Multimedia, and Charts development packages are required."
    exit 1
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
    "$SCRIPT_DIR/src/qtWrapper.cpp" "$SCRIPT_DIR/src/qtDjazair.cpp" \
    -o "$SCRIPT_DIR/$OUTPUT_LIB" \
    -L"$ROOT/build/bin" -ldjazair $QT_LIBS

echo "[OK] $OUTPUT_LIB built successfully."
