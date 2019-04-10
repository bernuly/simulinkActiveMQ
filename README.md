Simulink activMQ block
======================

This is a new ActivMQ for Simulink project using "pure" ActiveMQ (not the Smartbody version)
The starting point is the ActiveMQ codebase used in Unity

Example sender and receiver code in `apache-activemq-5.15.9-stomp`


## Requirement
- [APR](https://apr.apache.org/download.cgi)
- activemq-cpp-library

## Building

### Compiling the simulink block   
- use `combile.bat`

** was not able to get the CMake-based build to work**.
But, just in case, these are paths needed:
- MX_LIBRARY: "C:\Program Files\MATLAB\R2018b\extern\lib\win64\microsoft\libmx.lib"
- DATAARRAY_LIBRARY: "C:\Program Files\MATLAB\R2018b\extern\lib\win64\microsoft\libMatlabDataArray.lib"
- ENGINE_LIBRARY: "C:\Program Files\MATLAB\R2018b\extern\lib\win64\microsoft\libMatlabEngine.lib"
- MEX_LIBRARY: "C:\Program Files\MATLAB\R2018b\extern\lib\win64\microsoft\libmex.lib"

### apr
- **Compile using CMake**
- *standard compile with `nmake -f Makefile.win ARCH="x64 Release"` does not work for x64*
- output in `Release`

### activemq-cpp
- use Visual Studio project in `vs2010-build`
- Additional include dirs:
   - `..\src\main\activemq\cmsutil`
   - `..\..\apr-1.6.5\include\`

## Install
- use `install.bat`
- default install dir is `%USERPROFILE%\Documents\MATLAB\simulinkActiveMQ`
  
   

## Misc
- edit activeMQ.slx for chaning mask etc
- Issues with /FS
  C++ -> Command Line property -> Additional Options:  /FS