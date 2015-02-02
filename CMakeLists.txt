# CMake 3.1 was the first release to provide
# target_compile_features for setting C++11 flags
cmake_minimum_required(VERSION 3.1.0)

# If build type is not set, assume release by default
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "None Debug Release RelWithDebInfo MinSizeRel")
endif()

# The current version of the application is specified here exclusively
project(nitroshare VERSION 0.3.0 LANGUAGES CXX)

# Ensure that the required Qt libraries are available
find_package(Qt5Network 5.1 REQUIRED)
find_package(Qt5Widgets 5.1 REQUIRED)

# Set some flags that are needed for building Qt projects
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Include the subdirectory containing the source code for the application
add_subdirectory(src)

# Further customization is provided by platform-specific subdirectories
# We must manually compare CMAKE_SYSTEM_NAME for Mac and Linux since
# there are no predefined variables for them specifically
if(WIN32)
    add_subdirectory(windows)
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    add_subdirectory(mac)
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
    add_subdirectory(linux)
else()
    message(FATAL_ERROR "Building NitroShare on ${CMAKE_SYSTEM_NAME} is currently not supported.")
endif()

# Settings for CPack
set(CPACK_PACKAGE_NAME "NitroShare")
set(CPACK_PACKAGE_VENDOR "Nathan Osman")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Network File Transfer Application")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_EXECUTABLES "nitroshare;NitroShare")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "NitroShare")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")

include(CPack)
