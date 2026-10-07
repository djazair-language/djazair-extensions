@echo off
setlocal EnableDelayedExpansion

set "DJAZAIR_DIR=%~1"

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
    echo [ERROR] Djazair SDK not found.
    exit /b 1
)

set "INC_FLAGS=-I"%DJAZAIR_DIR%\include""
if exist "%DJAZAIR_DIR%\src\include" (
    set "INC_FLAGS=-I"%DJAZAIR_DIR%\src\include" -I"%DJAZAIR_DIR%\src\core" -I"%DJAZAIR_DIR%\src\libs""
)

set "LIB_DIR=%DJAZAIR_DIR%\lib"
if exist "%DJAZAIR_DIR%uildin\libdjazair.a" set "LIB_DIR=%DJAZAIR_DIR%uildin"

echo [INFO] Building regex extension...
gcc -shared -O2 -std=c99 ^
    %INC_FLAGS% ^
    regex_native.c ^
    -o regex.dll ^
    -L"%LIB_DIR%" -ldjazair -lregex

if errorlevel 1 (
    echo [ERROR] Build failed.
    exit /b 1
)
echo [OK] regex.dll built successfully.
