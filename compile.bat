@echo OFF
set MATLAB_DIR=C:\Program Files\MATLAB\R2018b
set MATLAB_ROOT=C:\PROGRA~1\MATLAB\R2018b
set WIN_SDK_DIR=C:\Program Files\Microsoft SDKs\Windows\v7.1\


set LIBDIRS=-Lactivemq-cpp-library-3.9.5\vs2010-build\x64\Release -Lapr-1.6.5\Release


set LIBS=-llibactivemq-cpp -llibapr-1
set INCL=-Iactivemq-cpp-library-3.9.5 -Iactivemq-cpp-library-3.9.5\src\main -Iapr-1.6.5 -Iapr-1.6.5\include -lWs2_32.lib


set FLAGS=-g -D_WIN64 -D_WINDOWS
REM -DNDEBUG


echo %LIBDIRS%
echo %LIBS%
echo %INCL%

call "%MATLAB_DIR%\bin\mex.bat" %FLAGS%  %LIBDIRS% %LIBS% %INCL% writeActiveMQ.cpp -outdir Release
call "%MATLAB_DIR%\bin\mex.bat" %FLAGS%  %LIBDIRS% %LIBS% %INCL% writeActiveMQ2.cpp -outdir Release

pause




  
