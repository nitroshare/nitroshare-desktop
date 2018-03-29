@ECHO OFF

REM Configure the application in the current directory
cmake ^
    -G"NMake Makefiles" ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DCMAKE_INSTALL_PREFIX=out ^
    %~dp0\.. || exit /b

REM Build and install the application
nmake || exit /b
nmake install || exit /b

REM Run windeployqt on nitroshare-ui
windeployqt ^
    --verbose 0 ^
    --no-angle ^
    --no-opengl-sw ^
    out/bin/nitroshare-ui || exit /b
