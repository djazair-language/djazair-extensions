#!/bin/sh
# ─────────────────────────────────────────────
#  build.sh  –  mysql extension for Djazair
#  Usage: ./build.sh <ROOT> [MYSQL_INCLUDE_DIR]
#    ROOT             = path to the djazair-language repo root
#    MYSQL_INCLUDE_DIR = optional override for MySQL headers
# ─────────────────────────────────────────────
set -e

# Locate Djazair SDK / Installation (General Use)
DJAZAIR_DIR="$1"
MYSQL_INC_OVERRIDE="$2"

if [ -z "$DJAZAIR_DIR" ] && [ -n "$DJAZAIR_HOME" ]; then
    DJAZAIR_DIR="$DJAZAIR_HOME"
fi

if [ -z "$DJAZAIR_DIR" ] && [ -n "$DJAZAIR_ROOT" ]; then
    DJAZAIR_DIR="$DJAZAIR_ROOT"
fi

if [ -z "$DJAZAIR_DIR" ]; then
    EXE_PATH="$(command -v djazair 2>/dev/null || true)"
    if [ -n "$EXE_PATH" ]; then
        BIN_DIR="$(dirname "$EXE_PATH")"
        if [ -f "$BIN_DIR/../../src/include/djazair_api.h" ]; then
            DJAZAIR_DIR="$(cd "$BIN_DIR/../.." && pwd)"
        elif [ -f "$BIN_DIR/../include/djazair_api.h" ]; then
            DJAZAIR_DIR="$(cd "$BIN_DIR/.." && pwd)"
        fi
    fi
fi

if [ -z "$DJAZAIR_DIR" ]; then
    SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
    for CANDIDATE in "$SCRIPT_DIR/../../djazair-language" "$SCRIPT_DIR/../../../djazair-language" "$SCRIPT_DIR/../djazair-language" "$SCRIPT_DIR/../.."; do
        if [ -f "$CANDIDATE/src/include/djazair_api.h" ]; then
            DJAZAIR_DIR="$(cd "$CANDIDATE" && pwd)"
            break
        fi
    done
fi

if [ -z "$DJAZAIR_DIR" ]; then
    echo "[ERROR] Djazair SDK / installation not found in PATH or DJAZAIR_HOME."
    echo "Please install Djazair and add it to your PATH, or set DJAZAIR_HOME."
    exit 1
fi

INC_FLAGS="-I$DJAZAIR_DIR/include"
if [ -d "$DJAZAIR_DIR/src/include" ]; then
    INC_FLAGS="-I$DJAZAIR_DIR/src/include -I$DJAZAIR_DIR/src/core -I$DJAZAIR_DIR/src/libs"
fi

LIB_DIR="$DJAZAIR_DIR/lib"
if [ -f "$DJAZAIR_DIR/build/bin/libdjazair.a" ] || [ -f "$DJAZAIR_DIR/build/bin/libdjazair.so" ]; then
    LIB_DIR="$DJAZAIR_DIR/build/bin"
fi

if ! command -v gcc >/dev/null 2>&1; then
    echo "[ERROR] gcc not found in PATH."
    echo "Install gcc (e.g. 'sudo apt install build-essential' on Debian/Ubuntu)."
    exit 1
fi

# ── Resolve MySQL include flags ───────────────────────────────────────────
MYSQL_CFLAGS=""
MYSQL_LIBS=""

if [ -n "$MYSQL_INC_OVERRIDE" ]; then
    # User provided explicit path
    MYSQL_CFLAGS="-I$MYSQL_INC_OVERRIDE"
    MYSQL_LIBS="-lmysqlclient"
elif command -v pkg-config >/dev/null 2>&1 && pkg-config --exists mysqlclient 2>/dev/null; then
    MYSQL_CFLAGS="$(pkg-config --cflags mysqlclient)"
    MYSQL_LIBS="$(pkg-config --libs mysqlclient)"
elif command -v pkg-config >/dev/null 2>&1 && pkg-config --exists mariadb 2>/dev/null; then
    MYSQL_CFLAGS="$(pkg-config --cflags mariadb)"
    MYSQL_LIBS="$(pkg-config --libs mariadb)"
elif command -v mysql_config >/dev/null 2>&1; then
    MYSQL_CFLAGS="$(mysql_config --cflags)"
    MYSQL_LIBS="$(mysql_config --libs)"
else
    # Fallback: search common paths
    for D in /usr/include/mysql /usr/local/include/mysql /opt/homebrew/include/mysql; do
        if [ -d "$D" ]; then
            MYSQL_CFLAGS="-I$D"
            break
        fi
    done
    MYSQL_LIBS="-lmysqlclient"
    if [ -z "$MYSQL_CFLAGS" ]; then
        echo "[ERROR] MySQL/MariaDB headers not found."
        echo "Install libmysqlclient-dev (Debian/Ubuntu) or mariadb-devel (Fedora)."
        echo "Or pass the include path as: ./build.sh '$ROOT' /path/to/mysql/include"
        exit 1
    fi
fi

echo "[INFO] MySQL CFLAGS: $MYSQL_CFLAGS"
echo "[INFO] MySQL LIBS:   $MYSQL_LIBS"

# ── OS detection for shared lib extension ─────────────────────────────────
OS="$(uname -s 2>/dev/null || echo "Linux")"
case "$OS" in
    Darwin*)  OUT="mysql.dylib" ; SHARED="-dynamiclib" ;;
    *)        OUT="mysql.so"    ; SHARED="-shared"     ;;
esac

echo "[INFO] Building mysql extension ($OUT)..."
gcc $SHARED -O2 -std=c99 \
    $INC_FLAGS \
    $MYSQL_CFLAGS \
    src/mysql.c src/mysql_conn.c src/mysql_query.c src/mysql_res.c \
    -o "$OUT" \
    -L"$LIB_DIR" -ldjazair $MYSQL_LIBS

echo "[OK] $OUT built successfully."
