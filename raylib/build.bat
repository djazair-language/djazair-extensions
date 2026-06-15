@echo off
set ROOT=%~1
gcc -shared -O2 -std=c99 -I"%ROOT%\src\include" -I"%ROOT%\src\core" -I"%ROOT%\src\libs" src\raylib.c src\raylib_camera.c src\raylib_core.c src\raylib_helpers.c src\raylib_media.c src\raylib_shapes.c -o raylib.dll -L"%ROOT%\build\bin" -ldjazair -lraylib -lgdi32 -lwinmm -luser32 -lshell32
