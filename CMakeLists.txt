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

# At the very minimum, Qt 5.4+ is required to compile the application
find_package(Qt5Network 5.4 REQUIRED)

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
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_subdirectory(libnitroshare)
add_subdirectory(daemon)
add_subdirectory(plugins)
