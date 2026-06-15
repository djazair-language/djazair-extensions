#!/bin/sh
ROOT="$1"
gcc -shared -O2 -std=c99 -I"$ROOT/src/include" -I"$ROOT/src/core" -I"$ROOT/src/libs" $(pkg-config --cflags mariadb 2>/dev/null || echo "-I/usr/include/mysql") src/mysql.c src/mysql_conn.c src/mysql_query.c src/mysql_res.c -o mysql.so -L"$ROOT/build/bin" -ldjazair $(pkg-config --libs mariadb 2>/dev/null || echo "-lmysqlclient")
