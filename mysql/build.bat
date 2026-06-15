@echo off
set ROOT=%~1
gcc -shared -O2 -std=c99 -I"%ROOT%\src\include" -I"%ROOT%\src\core" -I"%ROOT%\src\libs" -IC:/msys64/mingw64/include/mysql src\mysql.c src\mysql_conn.c src\mysql_query.c src\mysql_res.c -o mysql.dll -L"%ROOT%\build\bin" -ldjazair -lmysqlclient
