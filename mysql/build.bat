@echo off
setlocal EnableDelayedExpansion

:: ─────────────────────────────────────────────
::  build.bat  –  mysql extension for Djazair
::  Usage: build.bat <ROOT> [MYSQL_DIR]
::    ROOT      = path to the djazair-language repo root
::    MYSQL_DIR = path to MySQL/MariaDB include dir (optional)
::                Auto-detected from common MSYS2/MariaDB locations.
:: ─────────────────────────────────────────────

:: ── Locate Djazair SDK / Installation (General Use) ────────────────────────
set "DJAZAIR_DIR="
if not "%~1"=="" if exist "%~1\djazair_api.h" set "DJAZAIR_DIR=%~f1"
if not "%~1"=="" if exist "%~1\src\include\djazair_api.h" set "DJAZAIR_DIR=%~f1"

if "%DJAZAIR_DIR%"=="" if defined DJAZAIR_HOME set "DJAZAIR_DIR=%DJAZAIR_HOME%"
if "%DJAZAIR_DIR%"=="" if defined DJAZAIR_ROOT set "DJAZAIR_DIR=%DJAZAIR_ROOT%"

if "%DJAZAIR_DIR%"=="" (
    for /f "delims=" %%I in ('where djazair.exe 2^>nul') do (
        for %%A in ("%%~dpI..\..") do (
            if exist "%%~fA\src\include\djazair_api.h" set "DJAZAIR_DIR=%%~fA"
        )
        if "!DJAZAIR_DIR!"=="" (
            for %%A in ("%%~dpI..") do (
                if exist "%%~fA\include\djazair_api.h" set "DJAZAIR_DIR=%%~fA"
            )
        )
    )
)

if "%DJAZAIR_DIR%"=="" (
    for %%P in (
        "%~dp0..\..\djazair-language"
        "%~dp0..\..\..\djazair-language"
        "%~dp0..\djazair-language"
        "%~dp0..\.."
    ) do (
        if exist "%%~fP\src\include\djazair_api.h" (
            set "DJAZAIR_DIR=%%~fP"
            goto :found_djazair
        )
    )
)

:found_djazair
if "%DJAZAIR_DIR%"=="" (
    echo [ERROR] Djazair SDK / installation not found in PATH or DJAZAIR_HOME.
    echo Please install Djazair and add it to your PATH, or set DJAZAIR_HOME:
    echo   set DJAZAIR_HOME=C:\path\to\djazair
    exit /b 1
)

:: Resolve include and library flags
set "INC_FLAGS=-I"%DJAZAIR_DIR%\include""
if exist "%DJAZAIR_DIR%\src\include" (
    set "INC_FLAGS=-I"%DJAZAIR_DIR%\src\include" -I"%DJAZAIR_DIR%\src\core" -I"%DJAZAIR_DIR%\src\libs""
)

set "LIB_DIR=%DJAZAIR_DIR%\lib"
if exist "%DJAZAIR_DIR%\build\bin\libdjazair.a" (
    set "LIB_DIR=%DJAZAIR_DIR%\build\bin"
) else if exist "%DJAZAIR_DIR%\bin\libdjazair.a" (
    set "LIB_DIR=%DJAZAIR_DIR%\bin"
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
    %INC_FLAGS% ^
    -I"%MYSQL_INC%" ^
    src\mysql.c src\mysql_conn.c src\mysql_query.c src\mysql_res.c ^
    -o mysql.dll ^
    -L"%LIB_DIR%" -ldjazair -lmysqlclient

if errorlevel 1 (
    echo [ERROR] Build failed.
    exit /b 1
)

echo [OK] mysql.dll built successfully.
