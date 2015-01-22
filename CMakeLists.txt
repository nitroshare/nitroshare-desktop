# CMake 3.1 was the first release to provide
# target_compile_features for setting C++11 flags
cmake_minimum_required(VERSION 3.1.0)
project(nitroshare VERSION 0.3.0 LANGUAGES CXX)

# Ensure that the required Qt libraries are available
find_package(Qt5Network 5.1 REQUIRED)
find_package(Qt5Widgets 5.1 REQUIRED)

# Set a flag needed for building Qt projects
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Prepare a "config.h" file that contains version information
configure_file(src/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

# Add the source files
set(SRC
    src/connection/incomingconnection.cpp
    src/connection/outgoingconnection.cpp
    src/device/device.cpp
    src/device/devicedialog.cpp
    src/device/devicelistener.cpp
    src/device/devicemodel.cpp
    src/filesystem/bundle.cpp
    src/filesystem/file.cpp
    src/transfer/transfer.cpp
    src/transfer/transfermodel.cpp
    src/transfer/transferserver.cpp
    src/util/misc.cpp
    src/util/settings.cpp
    src/main.cpp
    src/nitroshare.cpp)

# Add the files that require MOC
qt5_wrap_cpp(MOC
    src/connection/connection.h
    src/connection/incomingconnection.h
    src/connection/outgoingconnection.h
    src/device/devicedialog.h
    src/device/devicelistener.h
    src/device/devicemodel.h
    src/transfer/transfer.h
    src/transfer/transfermodel.h
    src/transfer/transferserver.h
    src/util/settings.h
    src/nitroshare.h)

# Add the UI files
qt5_wrap_ui(UI
    src/device/devicedialog.ui)

# Add resource files
qt5_add_resources(QRC resources.qrc)

# Create a single executable
add_executable(nitroshare WIN32 ${SRC} ${MOC} ${UI} ${QRC})

# Specify the required Qt libraries and ensure the
# correct flags are set to enable C++11 features
target_link_libraries(nitroshare Qt5::Network Qt5::Widgets)
target_compile_features(nitroshare PRIVATE
    cxx_generalized_initializers
    cxx_lambdas
    cxx_nullptr)

# Install the application to the appropriate location
install(TARGETS nitroshare RUNTIME DESTINATION bin)
