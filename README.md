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

1. Install GCC and the Qt 5 development files:

        sudo apt-get install build-essential qtbase5-dev libqt5svg5 libgtk2.0-dev libappindicator-dev libnotify-dev

   Note: the last three packages add support for application indicators and are optional.

2. Change to the root of the source directory and build the project:

        qmake
        make

3. The NitroShare binary will be in the `out/install` directory and can be run with:

        out/install/nitroshare

4. To create a binary Debian package, ensure that the `devscripts` package is installed and run the following command:

        sudo apt-get install devscripts
        debuild -b

#### Windows 7, 8, & 8.1

1. Download and install the following tools:

    - [Visual Studio Express 2013 for Windows Desktop](http://go.microsoft.com/?linkid=9832280&clcid=0x409) [requires sign-in]
    - [Qt Online Installer for Windows](http://www.qt.io/download-open-source/)
    - [Inno Setup](http://www.jrsoftware.org/isinfo.php) [optional]

   Note: the last tool is only required if you wish to build a Windows EXE installer.

2. Ensure that the `bin` directory for Qt (and the installation directory of Inno Setup, if applicable) has been added to the `PATH` environment variable.

3. Open the appropriate command prompt for Visual C++. In Visual C++ 2013, these shortcuts are labeled as follows:

    - VS2013 x86 Native Tools Command Prompt
    - VS2013 x64 Cross Tools Command Prompt

4. Change to the root of the source directory and build the project

        qmake
        nmake

5. The NitroShare binary will be in the `out\install` directory and can be run with:

        out\install\nitroshare.exe

6. To create a Windows EXE installer, run the following command:

        nmake exe

> **Important:** By default, Visual C++ 2013 will build a binary that will not run on Windows XP. To avoid this behavior, ensure that the following environment variables are set before running `nmake`:
>
>     set CL=/D_USING_V110_SDK71_
>     set LINK=/SUBSYSTEM:WINDOWS,5.01

#### OS X 10.6, 10.7, & 10.8

1. Download and install the following tools:

    - [Xcode](https://itunes.apple.com/ca/app/xcode/id497799835?mt=12)
    - [Qt Online Installer for Mac](http://www.qt.io/download-open-source/)

2. Ensure that the `bin` directory for Qt has been added to `/etc/paths`.

3. Open the terminal app, change to the root source directory, and build the project:

        qmake
        make

4. The NitroShare bundle will be in the `out/install` directory and can be run with:

        open out/install/nitroshare.app

5. To create a compressed disk image (DMG), run the following command:

        make dmg
