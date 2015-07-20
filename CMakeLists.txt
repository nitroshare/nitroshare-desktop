cmake_minimum_required(VERSION 3.2.0)
project(NitroShare)

set(PROJECT_NAME "NitroShare")
set(PROJECT_DESCRIPTION "Network File Transfer Application")
set(PROJECT_AUTHOR "Nathan Osman")

set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 3)
set(PROJECT_VERSION_PATCH 1)
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

option(BUILD_APPINDICATOR "Application indicator support" OFF)
option(BUILD_LOCAL_API "Expose a local HTTP API" ON)
option(BUILD_UPDATE_CHECKER "Check for updates at startup" ON)

find_package(Qt5Network 5.1 REQUIRED)
find_package(Qt5Widgets 5.1 REQUIRED)
find_package(Qt5Svg 5.1 REQUIRED)

if(BUILD_APPINDICATOR)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(APPINDICATOR REQUIRED gtk+-2.0 appindicator-0.1 libnotify)
endif()

if(BUILD_LOCAL_API)
    find_package(QHttpEngine 0.1.0 REQUIRED)
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

add_subdirectory(src)
