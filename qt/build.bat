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

set "QT_INC=-IC:\msys64\mingw64\include -IC:\msys64\mingw64\include\QtWidgets -IC:\msys64\mingw64\include\QtCore -IC:\msys64\mingw64\include\QtGui -IC:\msys64\mingw64\include\QtUiTools -IC:\msys64\mingw64\include\QtMultimedia -IC:\msys64\mingw64\include\QtMultimediaWidgets"
set "QT_LIB=-LC:\msys64\mingw64\lib"

if defined QTDIR (
    if exist "%QTDIR%\include\QtWidgets" (
        set "QT_INC=-I%QTDIR%\include -I%QTDIR%\include\QtWidgets -I%QTDIR%\include\QtCore -I%QTDIR%\include\QtGui -I%QTDIR%\include\QtUiTools -I%QTDIR%\include\QtMultimedia -I%QTDIR%\include\QtMultimediaWidgets"
        set "QT_LIB=-L%QTDIR%\lib"
        echo [INFO] Found Qt via %%QTDIR%%: %QTDIR%
    )
)

echo [INFO] Compiling Qt dynamic extension (qt.dll)...

"%GXX%" -shared -O2 -std=c++17 ^
    "-I%ROOT%\src\include" ^
    "-I%ROOT%\src\core" ^
    "-I%ROOT%\src\libs" ^
    %QT_INC% -DQT_CHARTS_LIB ^
    src\qtWrapper.cpp src\qtDjazair.cpp ^
    -o qt.dll ^
    "-L%ROOT%\build\bin" %QT_LIB% ^
    -ldjazair -lQt5UiTools -lQt5MultimediaWidgets -lQt5Multimedia -lQt5Charts -lQt5Widgets -lQt5Gui -lQt5Xml -lQt5Core

if errorlevel 1 (
    echo [INFO] Trying Qt6 library linking...
    "%GXX%" -shared -O2 -std=c++17 ^
        "-I%ROOT%\src\include" ^
        "-I%ROOT%\src\core" ^
        "-I%ROOT%\src\libs" ^
        %QT_INC% -DQT_CHARTS_LIB ^
        src\qtWrapper.cpp src\qtDjazair.cpp ^
        -o qt.dll ^
        "-L%ROOT%\build\bin" %QT_LIB% ^
        -ldjazair -lQt6UiTools -lQt6MultimediaWidgets -lQt6Multimedia -lQt6Charts -lQt6Widgets -lQt6Gui -lQt6Xml -lQt6Core
)

if errorlevel 1 (
    echo [ERROR] Qt extension compilation failed. Please verify that Qt5 or Qt6 development packages are installed.
    exit /b 1
)

echo [OK] qt.dll built successfully.
