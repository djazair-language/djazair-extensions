@echo off
setlocal EnableDelayedExpansion

:: build.bat – zip extension for Djazair
:: Usage: build.bat <ROOT>
::   ROOT = path to the djazair-language repo root

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

where gcc >nul 2>&1
if errorlevel 1 (
    echo [ERROR] gcc not found in PATH.
    exit /b 1
)

echo [INFO] Building zip extension...
gcc -shared -O2 -std=c99 ^
    -I"%ROOT%\src\include" ^
    -I"%ROOT%\src\core" ^
    -I"%ROOT%\src\libs" ^
    src\zip.c ^
    -o zip.dll ^
    -L"%ROOT%\build\bin" -ldjazair ^
    -lz

if errorlevel 1 (
    echo [ERROR] Build failed.
    exit /b 1
)

echo [OK] zip.dll built successfully.
