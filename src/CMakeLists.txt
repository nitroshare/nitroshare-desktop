# Prepare a "config.h" file that contains version information
configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/config.h")

# Add the source files
set(SRC
    device/device.cpp
    device/devicedialog.cpp
    device/devicelistener.cpp
    device/devicemodel.cpp
    filesystem/bundle.cpp
    filesystem/fileinfo.cpp
    socket/socket.cpp
    socket/socketreader.cpp
    socket/socketstream.cpp
    socket/socketwriter.cpp
    transfer/transfer.cpp
    transfer/transferdelegate.cpp
    transfer/transfermodel.cpp
    transfer/transferserver.cpp
    transfer/transferwindow.cpp
    util/misc.cpp
    util/settings.cpp
    main.cpp
    nitroshare.cpp
    resources.qrc)

# Create a single executable
add_executable(nitroshare WIN32 ${SRC})

# Specify the required Qt libraries and ensure the
# correct flags are set to enable C++11 features
target_link_libraries(nitroshare Qt5::Network Qt5::Widgets)
target_compile_features(nitroshare PRIVATE
    cxx_delegating_constructors
    cxx_generalized_initializers
    cxx_lambdas
    cxx_nullptr)

# Install the application to the appropriate location
install(TARGETS nitroshare RUNTIME DESTINATION bin)
