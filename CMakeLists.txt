cmake_minimum_required(VERSION 3.2.0)
project(nitroshare)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 4)
set(VERSION_PATCH 0)
set(VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})

option(BUILD_TESTS "Build test suite" OFF)

find_package(Qt5Network 5.4 REQUIRED)

if(BUILD_TESTS)
    find_package(Qt5Test 5.4 REQUIRED)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_subdirectory(libnitroshare)
add_subdirectory(daemon)
add_subdirectory(plugins)
