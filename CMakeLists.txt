cmake_minimum_required(VERSION 3.2.0)
project(nitroshare)

# Point CMake to the custom modules
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# It would be nice if CMake offered a constant for Linux
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(LINUX 1)
endif()

set(PROJECT_NAME "NitroShare")
set(PROJECT_DESCRIPTION "Cross-platform network file transfer application")
set(PROJECT_AUTHOR "Nathan Osman")
set(PROJECT_URL "https://nitroshare.net")

set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 4)
set(PROJECT_VERSION_PATCH 0)
set(PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})

# At the very minimum, Qt 5.4+ is required
find_package(Qt5Network 5.4 REQUIRED)
find_package(Qt5Widgets 5.4)

# CLI and client are always available, UI is available if Qt5Widgets is available
option(BUILD_CLI "Build the NitroShare CLI app" ON)
option(BUILD_NMH "Build native messaging host for Chrome" ON)
option(BUILD_CLIENT "Build the NitroShare client app" ON)
if(Qt5Widgets_FOUND)
    option(BUILD_UI "Build the NitroShare UI app" ON)
endif()

option(ENABLE_TLS "Enable support for TLS" ON)

# Allow file installation directories to be customized
set(INSTALL_BIN_PATH bin CACHE STRING "Application installation directory")
set(INSTALL_LIB_PATH lib CACHE STRING "Shared library installation directory")
set(INSTALL_PLUGIN_PATH "${INSTALL_LIB_PATH}/nitroshare/plugins" CACHE STRING "Plugin installation directory")

# Output paths mirror the installation paths
if(CMAKE_CONFIGURATION_TYPES)
    set(OUTPUT_PREFIX "${CMAKE_BINARY_DIR}/out/$<CONFIG>")
else()
    set(OUTPUT_PREFIX "${CMAKE_BINARY_DIR}/out")
endif()
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_PREFIX}/${INSTALL_BIN_PATH}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_PREFIX}/${INSTALL_LIB_PATH}")
set(PLUGIN_OUTPUT_DIRECTORY "${OUTPUT_PREFIX}/${INSTALL_PLUGIN_PATH}")

# Set appropriate compiler flags, etc. (MSVC is already set to W3 and Clang
# issues enough warnings as it is)
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

# Documentation is disabled by default
option(BUILD_DOC "Build Doxygen documentation" OFF)
if(BUILD_DOC)
    find_package(Doxygen REQUIRED)
endif()

# Test suite is always disabled by default
option(BUILD_TESTS "Build test suite" OFF)
if(BUILD_TESTS)
    enable_testing()
    find_package(Qt5Test 5.4 REQUIRED)
endif()

# Enable local API if QHttpEngine is available
find_package(qhttpengine 1.0.0)
if(qhttpengine_FOUND)
    option(BUILD_API "Build local API plugin" ON)
endif()

# Enable mDNS discovery if QMdnsEngine is available
find_package(qmdnsengine)
if(qmdnsengine_FOUND)
    option(BUILD_MDNS "Build mDNS discovery plugin" ON)
endif()

if(WIN32)
    # windeployqt will be needed for all binary targets
    include(DeployQt)

    # Add a target for building an EXE on Windows
    find_package(InnoSetup QUIET)
    if(INNOSETUP_EXECUTABLE)
        configure_file(dist/setup.iss.in "${CMAKE_CURRENT_BINARY_DIR}/setup.iss")
        add_custom_target(exe
            COMMAND "${INNOSETUP_EXECUTABLE}"
                /Q
                /DTARGET_FILE_NAME="$<TARGET_FILE_NAME:nitroshare-ui>"
                \"${CMAKE_CURRENT_BINARY_DIR}/setup.iss\"
            COMMENT "Building installer..."
        )
    else()
        message(WARNING "InnoSetup not found - exe target disabled")
    endif()
endif()

# Add a target for building a DMG on macOS
if(APPLE)
    #...
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

# The library and plugins are always built
add_subdirectory(libnitroshare)
add_subdirectory(plugins)

if(BUILD_CLI)
    add_subdirectory(cli)
endif()

if(BUILD_CLIENT)
    add_subdirectory(client)
endif()

if(BUILD_NMH)
    add_subdirectory(nmh)
endif()

if(BUILD_UI)
    add_subdirectory(ui)
endif()
