@echo off
setlocal EnableDelayedExpansion

:: ─────────────────────────────────────────────
::  build.bat  –  mysql extension for Djazair
::  Usage: build.bat <ROOT> [MYSQL_DIR]
::    ROOT      = path to the djazair-language repo root
::    MYSQL_DIR = path to MySQL/MariaDB include dir (optional)
::                Auto-detected from common MSYS2/MariaDB locations.
:: ─────────────────────────────────────────────

if "%~1"=="" (
    echo [ERROR] Missing argument: ROOT path to djazair-language.
    echo Usage: build.bat ^<path\to\djazair-language^> [MYSQL_INCLUDE_DIR]
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
    echo Install MinGW-w64 or MSYS2 and add it to your PATH.
    exit /b 1
)

:: ── Locate MySQL include directory ─────────────────────────────────────────
if "%~2" NEQ "" (
    set "MYSQL_INC=%~2"
    goto :found_mysql
)

:: Auto-detect from common install locations
for %%D in (
    "C:\msys64\mingw64\include\mysql"
    "C:\msys64\mingw32\include\mysql"
    "C:\msys2\mingw64\include\mysql"
    "C:\Program Files\MySQL\MySQL Server 8.0\include"
    "C:\Program Files\MySQL\MySQL Server 5.7\include"
    "C:\Program Files\MariaDB 10.6\include\mysql"
    "C:\Program Files\MariaDB 10.11\include\mysql"
    "C:\Program Files (x86)\MySQL\MySQL Server 8.0\include"
) do (
    if exist "%%~D" (
        set "MYSQL_INC=%%~D"
        goto :found_mysql
    )
)

echo [ERROR] MySQL/MariaDB include directory not found.
echo Please pass it as the second argument:
echo   build.bat "%ROOT%" "C:\path\to\mysql\include"
exit /b 1

:found_mysql
echo [INFO] Using MySQL includes from: %MYSQL_INC%

echo [INFO] Building mysql extension...
gcc -shared -O2 -std=c99 ^
    -I"%ROOT%\src\include" ^
    -I"%ROOT%\src\core" ^
    -I"%ROOT%\src\libs" ^
    -I"%MYSQL_INC%" ^
    src\mysql.c src\mysql_conn.c src\mysql_query.c src\mysql_res.c ^
    -o mysql.dll ^
    -L"%ROOT%\build\bin" -ldjazair -lmysqlclient

if errorlevel 1 (
    echo [ERROR] Build failed.
    exit /b 1
)

echo [OK] mysql.dll built successfully.
