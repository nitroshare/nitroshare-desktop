cmake_minimum_required(VERSION 3.2.0)
project(NitroShare)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(PROJECT_NAME "NitroShare")
set(PROJECT_DESCRIPTION "Network File Transfer Application")
set(PROJECT_AUTHOR "Nathan Osman")
set(PROJECT_DOMAIN "nitroshare.net")
set(PROJECT_WEBSITE "https://${PROJECT_DOMAIN}")

set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 3)
set(PROJECT_VERSION_PATCH 2)
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

# In order for the Inno Setup script to work correctly, the runtime files need
# to be kept separate from the source code - bonus: it then becomes possible to
# run the executable directly from the directory after building.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out")

find_package(Qt5LinguistTools 5.1 REQUIRED)
find_package(Qt5Network 5.1 REQUIRED)
find_package(Qt5Widgets 5.1 REQUIRED)
find_package(Qt5Svg 5.1 REQUIRED)

# Qt5WinExtras is an optional dependency that allows the application to
# display transfer progress in the Windows taskbar.
if(WIN32)
    find_package(Qt5WinExtras)
endif()

# Qt5MacExtras is an optional dependency that allows the application to
# display transfer progress as a percentage in the dock.
if(APPLE)
    find_package(Qt5MacExtras)
endif()

# QHttpEngine is required in order to build the local HTTP API
find_package(QHttpEngine)

# libappindicator is an optional dependency that enables the application to
# determine at runtime if the current desktop environment supports using an
# application indicator instead of a QTrayIcon and use one instead
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    find_package(PkgConfig)
    if(PKG_CONFIG_FOUND)
        pkg_check_modules(APPINDICATOR gtk+-2.0 appindicator-0.1 libnotify)
    endif()
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

# Apple requires a couple extra flags
if(APPLE)
    set(CMAKE_CXX_FLAGS "-stdlib=libc++")
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.8")
endif()

# Since Qt and GTK+ both define the symbol "signals", it is necessary to
# ensure that it is not defined by Qt.
add_definitions(-DQT_NO_SIGNALS_SLOTS_KEYWORDS)

# Add the subdirectory containing the source code.
add_subdirectory(src)
