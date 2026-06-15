@echo off
set ROOT=%~1
gcc -shared -O2 -std=c99 -I"%ROOT%\src\include" -I"%ROOT%\src\core" -I"%ROOT%\src\libs" src\sqlite.c src\sqlite_db.c src\sqlite_stmt.c -o sqlite.dll -L"%ROOT%\build\bin" -ldjazair -lsqlite3
