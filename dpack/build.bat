@echo off
echo Building dpack stub...
gcc -O2 -Wall stub\stub.c -o stub\stub_win.exe -lws2_32
if %errorlevel% neq 0 (
    echo Error: Failed to compile stub.
    exit /b %errorlevel%
)
echo dpack stub built successfully.
exit /b 0
