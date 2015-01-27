## NitroShare

A cross-platform network file transfer application designed to make transferring any file to any device as painless as possible.

### Features

* Runs on Windows / Mac OS X / Linux
* Automatic discovery of devices on the local network
* Simple and intuitive user interface
* Transparent compression applied during transfer
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

### Build Instructions

#### Ubuntu 14.04, 14.10, & 15.04

1. None of the current releases ship CMake 3.1, so you will need to add my PPA:

        sudo add-apt-repository ppa:george-edison55/cmake-3.1
        sudo apt-get update

2. Install CMake, C++ compiler, and Qt 5 development files:

        sudo apt-get install cmake build-essential qtbase5-dev

3. Change to the root of the source directory and create a directory for building the project:

        mkdir build
        cd build

4. Run CMake followed by make:

        cmake ..
        make

5. The NitroShare binary will be in the current directory and can be run with:

        ./nitroshare

#### Windows 7, 8, & 8.1

TODO
