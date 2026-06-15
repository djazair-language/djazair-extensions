@echo off
set ROOT=%~1
g++ -shared -O2 -std=c++14 -I"%ROOT%\src\include" -I"%ROOT%\src\core" -I"%ROOT%\src\libs" src\webview_native.cc -o webview.dll -L"%ROOT%\build\bin" -ldjazair -lole32 -lshell32 -lshlwapi -luser32 -lversion -lcomdlg32 -ladvapi32 -Wl,--disable-dynamicbase
