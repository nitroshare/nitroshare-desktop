set(SRC
    main.cpp
)

add_executable(nitroshare-cli ${SRC})

set_target_properties(nitroshare-cli PROPERTIES
    CXX_STANDARD 11
)

target_include_directories(nitroshare-cli PUBLIC
    "${CMAKE_CURRENT_BINARY_DIR}"
)

target_link_libraries(nitroshare-cli nitroshare Qt5::Core)

install(TARGETS nitroshare-cli
    DESTINATION "${INSTALL_BIN_PATH}"
)

if(WIN32)
    # Run windeployqt on the binary
    include(DeployQt)
    windeployqt(nitroshare-cli)

    # If the exe target exists, add the binary as a dependency
    if(INNOSETUP_EXECUTABLE)
        add_dependencies(exe nitroshare-cli)
    endif()
endif()
