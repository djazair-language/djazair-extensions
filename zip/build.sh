#!/bin/sh
# build.sh – zip extension for Djazair
# Usage: ./build.sh <ROOT>
#   ROOT = path to the djazair-language repo root
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
    exit 1
fi

OS="$(uname -s 2>/dev/null || echo "Linux")"
case "$OS" in
    Darwin*)
        OUT="zip.dylib"
        SHARED="-dynamiclib"
        ;;
    MINGW*|MSYS*|CYGWIN*)
        OUT="zip.dll"
        SHARED="-shared"
        ;;
    *)
        OUT="zip.so"
        SHARED="-shared"
        ;;
esac

echo "[INFO] Building zip extension ($OUT)..."
gcc $SHARED -O2 -std=c99 \
    -I"$ROOT/src/include" \
    -I"$ROOT/src/core" \
    -I"$ROOT/src/libs" \
    src/zip.c \
    -o "$OUT" \
    -L"$ROOT/build/bin" -ldjazair -lz

echo "[OK] $OUT built successfully."
