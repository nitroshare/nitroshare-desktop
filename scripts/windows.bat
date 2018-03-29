@ECHO OFF

ECHO "Building installer for Windows"

REM Begin by configuring the project in the current directory
cmake ^
    -G"NMake Makefiles" ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DCMAKE_INSTALL_PREFIX=%cd%/out ^
    %~dp0\..

REM Build the entire project
nmake

REM Install the project to the out directory
nmake install
