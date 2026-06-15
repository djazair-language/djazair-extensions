#!/bin/sh
ROOT="$1"
gcc -shared -O2 -std=c99 -I"$ROOT/src/include" -I"$ROOT/src/core" -I"$ROOT/src/libs" src/curl.c src/curl_native.c -o curl.so -L"$ROOT/build/bin" -ldjazair -lcurl
