set(SRC
    main.cpp
)

add_executable(nitroshare-client ${SRC})

set_target_properties(nitroshare-client PROPERTIES
    CXX_STANDARD 11
    OUTPUT_NAME  nitroshare
)

target_link_libraries(nitroshare-client nitroshare Qt5::Core)

install(TARGETS nitroshare-client
    DESTINATION "${INSTALL_BIN_PATH}"
)

if(WIN32)
    # Run windeployqt on the binary
    include(DeployQt)
    windeployqt(nitroshare-client)

    # If the exe target exists, add the binary as a dependency
    if(INNOSETUP_EXECUTABLE)
        add_dependencies(exe nitroshare-client)
    endif()
endif()

if(APPLE)
    include(DeployQt)
    macdeployqt(nitroshare-client)
endif()
