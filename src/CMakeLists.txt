# Prepare a "config.h" file that contains version information
configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/config.h")

# On Windows, prepare a resource.rc script
if(WIN32)
    configure_file(resource.rc.in "${CMAKE_CURRENT_BINARY_DIR}/resource.rc")
endif()

# Add the source files
set(SRC
    application/application.cpp
    device/device.cpp
    device/devicedialog.cpp
    device/devicelistener.cpp
    device/devicemodel.cpp
    filesystem/bundle.cpp
    filesystem/fileinfo.cpp
    icon/icon.cpp
    icon/trayicon.cpp
    socket/socket.cpp
    socket/socketreader.cpp
    socket/socketstream.cpp
    socket/socketwriter.cpp
    transfer/transfer.cpp
    transfer/transferdelegate.cpp
    transfer/transfermodel.cpp
    transfer/transferserver.cpp
    transfer/transferwindow.cpp
    util/platform.cpp
    util/settings.cpp
    main.cpp
    resources.qrc)

# Include the IndicatorIcon class if enabled
if(BUILD_INDICATORICON)
    set(SRC ${SRC} icon/indicatoricon.cpp)
endif()

# On Windows, also include the resource file
if(WIN32)
    set(SRC ${SRC} ${CMAKE_CURRENT_BINARY_DIR}/resource.rc)
endif()

# Create a single executable and specify the required Qt libraries
add_executable(nitroshare WIN32 ${SRC})
target_link_libraries(nitroshare Qt5::Network Qt5::Widgets)

# Include the IndicatorIcon libraries if enabled
if(BUILD_INDICATORICON)
    target_link_libraries(nitroshare ${APPINDICATOR_LIBRARIES})
endif()

# Ensure the correct flags are set to enable C++11 features
target_compile_features(nitroshare PRIVATE
    cxx_delegating_constructors
    cxx_generalized_initializers
    cxx_lambdas
    cxx_nullptr)

# Install the application (and any required system libraries)
install(TARGETS nitroshare RUNTIME DESTINATION bin)
include(InstallRequiredSystemLibraries)

# On Windows, we must also install the Qt libraries
if(WIN32)
    # Determine the path to windeployqt by finding the path to qmake
    get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
    find_program(_windeployqt windeployqt HINTS "${_qt_bin_dir}")

    # Add a custom command that runs immediately after build, copying the
    # Qt libraries and their dependencies to a temporary directory
    add_custom_command(TARGET nitroshare POST_BUILD
        COMMAND ${_windeployqt}
            --dir "${CMAKE_BINARY_DIR}/qt"
            --no-compiler-runtime
            --verbose 0
            "$<TARGET_FILE:nitroshare>"
        VERBATIM)

    # Include the Qt libraries during installation
    install(DIRECTORY "${CMAKE_BINARY_DIR}/qt/" DESTINATION bin)
endif()
