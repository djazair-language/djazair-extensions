#!/bin/sh
# ─────────────────────────────────────────────
#  build.sh  –  webview extension for Djazair
#  Usage: ./build.sh <ROOT>
#    ROOT = path to the djazair-language repo root
#
#  Platform dependencies:
#    Linux   : libwebkit2gtk-4.0-dev, libgtk-3-dev
#    macOS   : WebKit.framework (built-in)
#    Windows : Edge WebView2 (via COM, linked with ole32/shell32)
# ─────────────────────────────────────────────
set -e

ROOT="$1"

if [ -z "$ROOT" ]; then
    echo "[ERROR] Missing argument: ROOT path to djazair-language."
    echo "Usage: ./build.sh <path/to/djazair-language>"
    exit 1
fi

if [ ! -d "$ROOT/src/include" ]; then
    echo "[ERROR] '$ROOT/src/include' not found. Is ROOT correct?"
    exit 1
fi

if ! command -v g++ >/dev/null 2>&1; then
    echo "[ERROR] g++ not found in PATH."
    echo "Install g++ (e.g. 'sudo apt install g++' on Debian/Ubuntu)."
    exit 1
fi

# ── OS-specific settings ──────────────────────────────────────────────────
OS="$(uname -s 2>/dev/null || echo "Linux")"
case "$OS" in
    Darwin*)
        OUT="webview.dylib"
        SHARED="-dynamiclib"
        PLATFORM_FLAGS=""
        PLATFORM_LIBS="-framework WebKit -framework Cocoa"
        ;;
    MINGW*|MSYS*|CYGWIN*)
        OUT="webview.dll"
        SHARED="-shared"
        PLATFORM_FLAGS="-DUNICODE -D_UNICODE -Wl,--disable-dynamicbase"
        PLATFORM_LIBS="-lole32 -lshell32 -lshlwapi -luser32 -lversion -lcomdlg32 -ladvapi32"
        ;;
    *)
        # Linux — requires libwebkit2gtk-4.0-dev and libgtk-3-dev
        OUT="webview.so"
        SHARED="-shared"
        PLATFORM_FLAGS=""
        if command -v pkg-config >/dev/null 2>&1 && pkg-config --exists webkit2gtk-4.0 2>/dev/null; then
            PLATFORM_LIBS="$(pkg-config --cflags --libs webkit2gtk-4.0 gtk+-3.0)"
        else
            echo "[WARNING] webkit2gtk-4.0 not found via pkg-config."
            echo "Install: sudo apt install libwebkit2gtk-4.0-dev libgtk-3-dev"
            PLATFORM_LIBS="-lwebkit2gtk-4.0 -lgtk-3"
        fi
        ;;
esac

echo "[INFO] Platform: $OS"
echo "[INFO] Building webview extension ($OUT)..."

g++ $SHARED -O2 -std=c++14 \
    -I"$ROOT/src/include" \
    -I"$ROOT/src/core" \
    -I"$ROOT/src/libs" \
    src/webview_native.cc \
    -o "$OUT" \
    -L"$ROOT/build/bin" -ldjazair \
    $PLATFORM_LIBS $PLATFORM_FLAGS

echo "[OK] $OUT built successfully."
