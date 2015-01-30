# CMake 3.1 was the first release to provide
# target_compile_features for setting C++11 flags
cmake_minimum_required(VERSION 3.1.0)
project(nitroshare VERSION 0.3.0 LANGUAGES CXX)

# Ensure that the required Qt libraries are available
find_package(Qt5Network 5.1 REQUIRED)
find_package(Qt5Widgets 5.1 REQUIRED)

# Set a flag needed for building Qt projects
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Include each applicable subdirectory in the build
add_subdirectory(src)

if("${CMAKE_SYSTEM_NAME}" MATCHES "Windows")
    add_subdirectory(windows)
elseif("${CMAKE_SYSTEM_NAME}" MATCHES "Darwin")
    add_subdirectory(mac)
elseif("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
    add_subdirectory(linux)
else()
    message(FATAL_ERROR "Building NitroShare on ${CMAKE_SYSTEM_NAME} is currently not supported.")
endif()
