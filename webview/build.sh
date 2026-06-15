#!/bin/sh
ROOT="$1"
g++ -shared -O2 -std=c++14 -I"$ROOT/src/include" -I"$ROOT/src/core" -I"$ROOT/src/libs" src/webview_native.cc -o webview.so -L"$ROOT/build/bin" -ldjazair $(pkg-config --cflags --libs webview 2>/dev/null || echo "-lwebview")
