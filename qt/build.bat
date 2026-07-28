@echo off
setlocal EnableDelayedExpansion

if "%~1"=="" (
    echo [ERROR] Missing argument: ROOT path to djazair-language.
    echo Usage: build.bat ^<path\to\djazair-language^>
    exit /b 1
)

set "ROOT=%~f1"

if not exist "%ROOT%\src\include" (
    echo [ERROR] "%ROOT%\src\include" not found. Is ROOT correct?
    exit /b 1
)

echo [INFO] Auto-detecting C++ compiler and Qt installation...

set "GXX=g++"
if exist "C:\msys64\mingw64\bin\g++.exe" (
    set "GXX=C:\msys64\mingw64\bin\g++.exe"
)

echo [INFO] Using C++ Compiler: %GXX%

set "QT_INC="
set "QT_LIB="
set "QT_FOUND=0"

if defined QTDIR (
    if exist "%QTDIR%\include\QtWidgets" (
        set "QT_INC=-I"%QTDIR%\include" -I"%QTDIR%\include\QtWidgets" -I"%QTDIR%\include\QtCore" -I"%QTDIR%\include\QtGui""
        set "QT_LIB=-L"%QTDIR%\lib""
        set "QT_FOUND=1"
        echo [INFO] Found Qt via %%QTDIR%%: %QTDIR%
    )
)

if "%QT_FOUND%"=="0" (
    if exist "C:\msys64\mingw64\include\QtWidgets" (
        set "QT_INC=-I"C:\msys64\mingw64\include" -I"C:\msys64\mingw64\include\QtWidgets" -I"C:\msys64\mingw64\include\QtCore" -I"C:\msys64\mingw64\include\QtGui""
        set "QT_LIB=-L"C:\msys64\mingw64\lib""
        set "QT_FOUND=1"
        echo [INFO] Found Qt via MSYS2 MinGW system directory.
    )
)

echo [INFO] Compiling Qt dynamic extension (qt.dll)...

"%GXX%" -shared -O2 -std=c++17 ^
    -I"%ROOT%\src\include" ^
    -I"%ROOT%\src\core" ^
    -I"%ROOT%\src\libs" ^
    %QT_INC% ^
    src\qt_wrapper.cpp src\qt_djazair.cpp ^
    -o qt.dll ^
    -L"%ROOT%\build\bin" %QT_LIB% ^
    -ldjazair -lQt5Widgets -lQt5Core -lQt5Gui

if errorlevel 1 (
    echo [INFO] Trying Qt6 library linking...
    "%GXX%" -shared -O2 -std=c++17 ^
        -I"%ROOT%\src\include" ^
        -I"%ROOT%\src\core" ^
        -I"%ROOT%\src\libs" ^
        %QT_INC% ^
        src\qt_wrapper.cpp src\qt_djazair.cpp ^
        -o qt.dll ^
        -L"%ROOT%\build\bin" %QT_LIB% ^
        -ldjazair -lQt6Widgets -lQt6Core -lQt6Gui
)

if errorlevel 1 (
    echo [ERROR] Qt extension compilation failed. Please verify that Qt5 or Qt6 development packages are installed.
    exit /b 1
)

echo [OK] qt.dll built successfully.
