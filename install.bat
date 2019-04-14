@echo OFF

set INSTALL_DIR=%USERPROFILE%\Documents\MATLAB\simulinkActiveMQ
echo copying files to %INSTALL_DIR%


mkdir %INSTALL_DIR%
copy simulink_files\activeMQ.slx      %INSTALL_DIR%
copy simulink_files\slblocks.m        %INSTALL_DIR%
copy simulink_files\activeMQ.png      %INSTALL_DIR%
copy Release\writeActiveMQ.mexw64  %INSTALL_DIR%
copy Release\writeActiveMQ2.mexw64  %INSTALL_DIR%
copy apr-1.6.5\Release\libapr-1.dll          %INSTALL_DIR%


pause
