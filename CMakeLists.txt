cmake_minimum_required(VERSION 3.2.0)
project(nitroshare)

# It would be nice if CMake offered a constant for Linux
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(LINUX 1)
endif()

set(VERSION_MAJOR 0)
set(VERSION_MINOR 4)
set(VERSION_PATCH 0)
set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

# At the very minimum, Qt 5.4+ is required to compile the daemon
find_package(Qt5Network 5.4 REQUIRED)

# QtWidgets is required to compile the client
find_package(Qt5Widgets 5.4)
if(Qt5Widgets_FOUND)
    option(BUILD_CLIENT "Build the NitroShare client" ON)
endif()

# Allow file installation directories to be customized
set(INSTALL_BIN_PATH bin CACHE STRING "Application installation directory")
set(INSTALL_LIB_PATH lib CACHE STRING "Shared library installation directory")
if(WIN32)
    set(INSTALL_PLUGIN_PATH "${INSTALL_BIN_PATH}/plugins" CACHE STRING "Plugin installation directory")
elseif(LINUX)
    set(INSTALL_PLUGIN_PATH "${INSTALL_LIB_PATH}/nitroshare/plugins" CACHE STRING "Plugin installation directory")
endif()

# Output paths mirror the installation paths
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out/${INSTALL_BIN_PATH}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out/${INSTALL_LIB_PATH}")
set(PLUGIN_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/out/${INSTALL_PLUGIN_PATH}")

# Test suite is always disabled by default
option(BUILD_TESTS "Build test suite" OFF)
if(BUILD_TESTS)
    enable_testing()
    find_package(Qt5Test 5.4 REQUIRED)
endif()

# Enable local API if QHttpEngine is available
find_package(QHttpEngine)
if(QHttpEngine_FOUND)
    option(BUILD_API "Build local API" ON)
endif()

# Enable indicator if GTK, appindicator, and libnotify are present
if(LINUX)
    find_package(PkgConfig)
    if(PKG_CONFIG_FOUND)
        pkg_check_modules(appindicator gtk+-2.0 appindicator-0.1 libnotify)
        if(appindicator_FOUND)
            option(BUILD_INDICATOR "Build application indicator" ON)
        endif()
    endif()
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_subdirectory(libnitroshare)
add_subdirectory(plugins)
add_subdirectory(daemon)

if(BUILD_CLIENT)
    add_subdirectory(client)
endif()
