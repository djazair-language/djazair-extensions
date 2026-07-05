@echo off
setlocal

start "KasbahServer" /B "D:\Programing\Djazair Programming Language\djazair-language\build\bin\djazair.exe" "D:\Programing\Djazair Programming Language\djazair-extensions\kasbah\examples\01_hello.dz"
timeout /t 3 /nobreak >nul

echo --- Testing GET / ---
curl -s http://localhost:8080/ 2>&1
echo.
echo --- Testing GET /json ---
curl -s http://localhost:8080/json 2>&1
echo.

taskkill /F /IM djazair.exe 2>nul
echo --- Done ---
