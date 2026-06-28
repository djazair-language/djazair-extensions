@echo off
setlocal EnableDelayedExpansion

:: ─────────────────────────────────────────────
::  build.bat  –  curl extension for Djazair
::  Usage: build.bat <ROOT>
::    ROOT = path to the djazair-language repo root
:: ─────────────────────────────────────────────

if "%~1"=="" (
    echo [ERROR] Missing argument: ROOT path to djazair-language.
    echo Usage: build.bat ^<path\to\djazair-language^>
    exit /b 1
)

set "ROOT=%~1"

:: Verify ROOT looks correct
if not exist "%ROOT%\src\include" (
    echo [ERROR] "%ROOT%\src\include" not found. Is ROOT correct?
    exit /b 1
)

:: Check for gcc
where gcc >nul 2>&1
if errorlevel 1 (
    echo [ERROR] gcc not found in PATH.
    echo Install MinGW-w64 or MSYS2 and add it to your PATH.
    exit /b 1
)

echo [INFO] Building curl extension...
gcc -shared -O2 -std=c99 ^
    -I"%ROOT%\src\include" ^
    -I"%ROOT%\src\core" ^
    -I"%ROOT%\src\libs" ^
    src\curl.c src\curl_native.c ^
    -o curl.dll ^
    -L"%ROOT%\build\bin" -ldjazair -lcurl

if errorlevel 1 (
    echo [ERROR] Build failed.
    exit /b 1
)

echo [OK] curl.dll built successfully.
