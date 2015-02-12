## NitroShare

A cross-platform network file transfer application designed to make transferring any file to any device as painless as possible.

### Features

* Runs on Windows / Mac OS X / Linux
* Automatic discovery of devices on the local network
* Simple and intuitive user interface
* Transfer entire directories
* Completely free and open-source

### Build Requirements

The requirements for building NitroShare are as follows:

* CMake 3.1+
* C++ compiler with support for C++11:
    * Microsoft Visual C++ 2013+
    * GCC 4.7+
    * Clang 3.1+
* Qt 5.1+

In order to use [application indicators](https://unity.ubuntu.com/projects/appindicators/) on supported Linux platforms, you also need the following development packages installed:

* GTK+ 2
* libappindicator
* libnotify

### Build Instructions

The instructions below describe the build process for each of the supported platforms.

#### Ubuntu 14.04, 14.10, & 15.04

1. None of the current releases ship CMake 3.1, so you will need to add my PPA:

        sudo add-apt-repository ppa:george-edison55/cmake-3.x
        sudo apt-get update

2. Install CMake, C++ compiler, and Qt 5 development files:

        sudo apt-get install cmake build-essential qtbase5-dev libqt5svg5 libgtk2.0-dev libappindicator-dev libnotify-dev

   Note: the last three packages add support for application indicators and are optional.

3. Change to the root of the source directory and create a directory for building the project:

        mkdir build
        cd build

4. Run CMake followed by make:

        cmake ..
        make

5. The NitroShare binary will be in the `src` directory and can be run with:

        src/nitroshare

#### Windows 7, 8, & 8.1

1. Download and install the following tools:

    - [CMake Win32 Installer](http://www.cmake.org/download/)
    - [Visual Studio Express 2013 for Windows Desktop](http://go.microsoft.com/?linkid=9832280&clcid=0x409) [requires sign-in]
    - [Qt Online Installer for Windows](http://www.qt.io/download-open-source/#section-2)

2. Ensure that the `bin` directory for both CMake and Qt have been added to the `PATH` environment variable.

3. Open the appropriate command prompt for Visual C++. In Visual C++ 2013, these shortcuts are labeled as follows:

    - VS2013 x86 Native Tools Command Prompt
    - VS2013 x64 Cross Tools Command Prompt

4. Change to the root of the source directory and create a directory for building the project:

        mkdir build
        cd build

5. Run CMake, being sure to specify the `NMake Makefiles` generator:

        cmake -G "NMake Makefiles" ..

6. Run `nmake` to build the executable:

        nmake

7. The NitroShare binary will be in the `src` directory and can be run with:

        src\nitroshare.exe

8. If NSIS is installed, you can build a standalone installer with the following command:

        nmake package

> **Important:** By default, Visual C++ 2013 will build a binary that will not run on Windows XP. To avoid this behavior, ensure that the following environment variables are set before running `nmake`:
>
>     set CL=/D_USING_V110_SDK71_
>     set LINK=/SUBSYSTEM:WINDOWS,5.01

#### OS X 10.6, 10.7, & 10.8

TODO

