@echo off
setlocal EnableDelayedExpansion

:: ─────────────────────────────────────────────
::  build.bat  –  webview extension for Djazair
::  Usage: build.bat <ROOT>
::    ROOT = path to the djazair-language repo root
::  Requires: g++ (MinGW-w64 / MSYS2)
:: ─────────────────────────────────────────────

if "%~1"=="" (
    echo [ERROR] Missing argument: ROOT path to djazair-language.
    echo Usage: build.bat ^<path\to\djazair-language^>
    exit /b 1
)

set "ROOT=%~1"

if not exist "%ROOT%\src\include" (
    echo [ERROR] "%ROOT%\src\include" not found. Is ROOT correct?
    exit /b 1
)

where g++ >nul 2>&1
if errorlevel 1 (
    echo [ERROR] g++ not found in PATH.
    echo Install MinGW-w64 or MSYS2 and add it to your PATH.
    exit /b 1
)

echo [INFO] Building webview extension...
g++ -shared -O2 -std=c++14 -DUNICODE -D_UNICODE ^
    -I"%ROOT%\src\include" ^
    -I"%ROOT%\src\core" ^
    -I"%ROOT%\src\libs" ^
    src\webview_native.cc ^
    -o webview.dll ^
    -L"%ROOT%\build\bin" -ldjazair ^
    -lole32 -lshell32 -lshlwapi -luser32 -lversion -lcomdlg32 -ladvapi32 ^
    -Wl,--disable-dynamicbase

if errorlevel 1 (
    echo [ERROR] Build failed.
    exit /b 1
)

echo [OK] webview.dll built successfully.
