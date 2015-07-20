## NitroShare

[![Current tagged release](https://img.shields.io/github/tag/nitroshare/nitroshare.svg)](https://github.com/nitroshare/nitroshare-desktop/releases/latest)
[![](http://img.shields.io/badge/license-MIT-blue.svg?style=flat)](http://opensource.org/licenses/MIT)
[![Join the chat at https://gitter.im/nitroshare/nitroshare-desktop](http://img.shields.io/badge/gitter-JOIN_CHAT-green.svg?style=flat)](https://gitter.im/nitroshare/nitroshare-desktop?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

A cross-platform network file transfer application designed to make transferring any file to any device as painless as possible.

### Features

* Runs on Windows / Mac OS X / Linux
* Automatic discovery of devices on the local network
* Simple and intuitive user interface
* Transfer entire directories
* Completely free and open-source

### Build Requirements

The requirements for building NitroShare are as follows:

* CMake 3.2.0+
* C++ compiler with support for C++11:
    * Microsoft Visual C++ 2013+
    * GCC 4.7+
    * Clang 3.1+
* Qt 5.1+

In order to use [application indicators](https://unity.ubuntu.com/projects/appindicators/) on supported Linux platforms, you will need the following development packages installed:

* GTK+ 2
* libappindicator
* libnotify

In order to use the local API, you will need [QHttpEngine](https://github.com/nitroshare/qhttpengine) 0.1.0+ installed.

### Building the Application

The instructions below describe the build process for each of the supported platforms.

#### Ubuntu 14.04, 14.10, 15.04, & 15.10

1. Versions of Ubuntu prior to 15.10 (Wily Werewolf) include an outdated version of CMake and therefore you will need to add the following PPA before installing CMake:

        sudo add-apt-repository ppa:george-edison55/cmake-3.x
        sudo apt-get update

2. Install GCC, CMake, and the Qt 5 development files:

        sudo apt-get install build-essential cmake qtbase5-dev libqt5svg5

   To enable support for application indicators, you will need to install the following packages:

        sudo apt-get install libgtk2.0-dev libappindicator-dev libnotify-dev

   To enable support for the local API, you will need to add a PPA and install the following package:

        sudo add-apt-repository ppa:george-edison55/nitroshare
        sudo apt-get update
        sudo apt-get install libqhttpengine-dev

2. Change to the root of the source directory and build the project:

        mkdir build && cd build
        cmake -DBUILD_APPINDICATOR=ON -DBUILD_LOCAL_API=ON ..
        make

   To enable automatic updates, include the `-DBUILD_UPDATE_CHECKER=ON` flag.

3. The NitroShare binary will be in the `src/` directory and can be run with:

        src/nitroshare

#### Windows 7, 8, & 8.1

> **Warning!** these instructions are out-of-date!

1. Download and install the following tools:

    - [Visual Studio Express 2013 for Windows Desktop](http://go.microsoft.com/?linkid=9832280&clcid=0x409) [requires sign-in]
    - [Qt Online Installer for Windows](http://www.qt.io/download-open-source/)

2. Ensure that the `bin` directory for Qt has been added to the `PATH` environment variable.

3. Open the appropriate command prompt for Visual C++. In Visual C++ 2013, these shortcuts are labeled as follows:

    - VS2013 x86 Native Tools Command Prompt
    - VS2013 x64 Cross Tools Command Prompt

4. Change to the root of the source directory and build the project

        qmake
        nmake

5. The NitroShare binary will be in the `out\install` directory and can be run with:

        out\install\nitroshare.exe

> **Important:** By default, Visual C++ 2013 will build a binary that will not run on Windows XP. To avoid this behavior, ensure that the following environment variables are set before running `nmake`:
>
>     set CL=/D_USING_V110_SDK71_
>     set LINK=/SUBSYSTEM:WINDOWS,5.01

#### OS X 10.7, 10.8, 10.9, & 10.10

> **Warning!** these instructions are out-of-date!

1. Download and install the following tools:

    - [Xcode](https://itunes.apple.com/ca/app/xcode/id497799835?mt=12)
    - [Qt Online Installer for Mac](http://www.qt.io/download-open-source/)

2. Ensure that the `bin` directory for Qt has been added to `/etc/paths`.

3. Open the terminal app, change to the root source directory, and build the project:

        qmake
        make

4. The NitroShare bundle will be in the `out/install` directory and can be run with:

        open out/install/nitroshare.app

### Building the Installers and Packages

The instructions below describe the steps necessary to create installers or packages on each of the supported platforms.

#### Ubuntu

Ensure that the `devscripts` package is installed and build the package by running the following commands:

    sudo apt-get install devscripts
    debuild -b

#### Windows

Building the installers for 32 and 64-bit editions of Windows requires [Inno Setup](http://www.jrsoftware.org/isinfo.php). You also need to ensure that the Inno Setup installation directory is added to the `PATH` environment variable.

> **Important:** The Express editions of Visual C++ do not include the redistributable installers. You will need to download the appropriate installers for the x86 and x64 architectures. (The Visual C++ 2013 Redistributable can be downloaded [here](http://www.microsoft.com/en-us/download/details.aspx?id=40784).) Locate the Visual C++ `redist` directory and create a directory inside named "1033". Place the two installers inside this directory.

Run the following command to build a Windows EXE installer:

    nmake exe

#### OS X

Run the following command to build a compressed disk image:

    make dmg
