cmake_minimum_required(VERSION 3.2.0)
project(NitroShare)

set(PROJECT_NAME "NitroShare")
set(PROJECT_DESCRIPTION "Network File Transfer Application")
set(PROJECT_AUTHOR "Nathan Osman")
set(PROJECT_DOMAIN "nitroshare.net")
set(PROJECT_WEBSITE "https://${PROJECT_DOMAIN}")

set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 3)
set(PROJECT_VERSION_PATCH 1)
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

include(GNUInstallDirs)

find_package(Qt5Network 5.1 REQUIRED)
find_package(Qt5Widgets 5.1 REQUIRED)
find_package(Qt5Svg 5.1 REQUIRED)


if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    find_package(PkgConfig)
    if(PKG_CONFIG_FOUND)
        pkg_check_modules(APPINDICATOR gtk+-2.0 appindicator-0.1 libnotify)
        pkg_check_modules(UNITY REQUIRED unity)
    endif()
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

add_subdirectory(dist)
add_subdirectory(src)
