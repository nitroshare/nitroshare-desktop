# CMake 3.1 was the first release to provide
# target_compile_features for setting C++11 flags
cmake_minimum_required(VERSION 3.1.0)

# If build type is not set, assume release by default
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "None Debug Release RelWithDebInfo MinSizeRel")
endif()

# The current version of the application is specified here exclusively
project(nitroshare VERSION 0.3.0 LANGUAGES CXX)

# We currently support a total of three platforms:
# - Windows XP / Vista / 7 / 8
# - OS X 10.6, 10.7, and 10.8
# - Linux (tested on Ubuntu)
if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    set(MAC 1)
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(LINUX 1)
elseif(NOT WIN32)
    message(FATAL_ERROR "Building NitroShare on ${CMAKE_SYSTEM_NAME} is currently not supported.")
endif()

# Ensure that the required Qt libraries are available
find_package(Qt5Network 5.1 REQUIRED)
find_package(Qt5Widgets 5.1 REQUIRED)

# If pkg-config and the appindicator libraries are available, build the IndicatorIcon class
find_package(PkgConfig)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(APPINDICATOR gtk+-2.0 appindicator-0.1 libnotify)
    if(APPINDICATOR_FOUND)
        message(STATUS "IndicatorIcon class will be built")
        include_directories(${APPINDICATOR_INCLUDE_DIRS})
        link_directories(${APPINDICATOR_LIBRARY_DIRS})
        set(BUILD_INDICATORICON 1)
    else()
        message(STATUS "IndicatorIcon class will not be built")
    endif()
endif()

# Set some flags that are needed for building Qt projects
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Include the subdirectory containing the source code for the application
add_subdirectory(src)

# On Linux, there are a few extra files that need to be installed
if(LINUX)
    add_subdirectory(linux)
endif()

# Generic settings for CPack
set(CPACK_PACKAGE_NAME "NitroShare")
set(CPACK_PACKAGE_VENDOR "Nathan Osman")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Network File Transfer Application")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_EXECUTABLES "nitroshare;NitroShare")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "NitroShare")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")

# NSIS settings for CPack
set(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}\\\\res\\\\nsis-icon.bmp")

# WiX (Windows installer) settings for CPack
set(CPACK_WIX_UPGRADE_GUID "51AA6F60-87CD-4C52-8CDF-97F784B276C0")
set(CPACK_WIX_UI_BANNER "${CMAKE_CURRENT_SOURCE_DIR}\\\\res\\\\wix-banner.bmp")
set(CPACK_WIX_UI_DIALOG "${CMAKE_CURRENT_SOURCE_DIR}\\\\res\\\\wix-dialog.bmp")

include(CPack)
