#!/bin/sh
# ─────────────────────────────────────────────
#  build.sh  –  mysql extension for Djazair
#  Usage: ./build.sh <ROOT> [MYSQL_INCLUDE_DIR]
#    ROOT             = path to the djazair-language repo root
#    MYSQL_INCLUDE_DIR = optional override for MySQL headers
# ─────────────────────────────────────────────
set -e

ROOT="$1"
MYSQL_INC_OVERRIDE="$2"

if [ -z "$ROOT" ]; then
    echo "[ERROR] Missing argument: ROOT path to djazair-language."
    echo "Usage: ./build.sh <path/to/djazair-language> [mysql_include_dir]"
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
    -I"$ROOT/src/include" \
    -I"$ROOT/src/core" \
    -I"$ROOT/src/libs" \
    $MYSQL_CFLAGS \
    src/mysql.c src/mysql_conn.c src/mysql_query.c src/mysql_res.c \
    -o "$OUT" \
    -L"$ROOT/build/bin" -ldjazair $MYSQL_LIBS

echo "[OK] $OUT built successfully."
