#!/bin/sh
# ─────────────────────────────────────────────
#  build.sh  –  raylib extension for Djazair
#  Usage: ./build.sh <ROOT>
#    ROOT = path to the djazair-language repo root
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

if ! command -v gcc >/dev/null 2>&1; then
    echo "[ERROR] gcc not found in PATH."
    echo "Install gcc (e.g. 'sudo apt install build-essential' on Debian/Ubuntu)."
    exit 1
fi

# ── OS-specific settings ──────────────────────────────────────────────────
OS="$(uname -s 2>/dev/null || echo "Linux")"
case "$OS" in
    Darwin*)
        OUT="raylib.dylib"
        SHARED="-dynamiclib"
        # macOS: uses Cocoa/OpenGL frameworks instead of Win32 libs
        PLATFORM_LIBS="-framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo"
        ;;
    MINGW*|MSYS*|CYGWIN*)
        # Running bash on Windows (e.g. Git Bash)
        OUT="raylib.dll"
        SHARED="-shared"
        PLATFORM_LIBS="-lgdi32 -lwinmm -luser32 -lshell32"
        ;;
    *)
        # Linux
        OUT="raylib.so"
        SHARED="-shared"
        # Try pkg-config first, fall back to common X11/GL libs
        if command -v pkg-config >/dev/null 2>&1 && pkg-config --exists raylib 2>/dev/null; then
            PLATFORM_LIBS="$(pkg-config --libs raylib)"
        else
            PLATFORM_LIBS="-lGL -lm -lpthread -ldl -lrt -lX11"
        fi
        ;;
esac

echo "[INFO] Platform: $OS"
echo "[INFO] Building raylib extension ($OUT)..."

gcc $SHARED -O2 -std=c99 \
    -I"$ROOT/src/include" \
    -I"$ROOT/src/core" \
    -I"$ROOT/src/libs" \
    src/raylib.c src/raylib_camera.c src/raylib_core.c \
    src/raylib_helpers.c src/raylib_media.c src/raylib_shapes.c \
    -o "$OUT" \
    -L"$ROOT/build/bin" -ldjazair -lraylib \
    $PLATFORM_LIBS

echo "[OK] $OUT built successfully."
