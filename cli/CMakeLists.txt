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
    # Ensure the Qt runtime libraries are installed
    include(Windeployqt)
    windeployqt(nitroshare-cli ${INSTALL_BIN_PATH})
endif()

if(APPLE)
    # Ensure the Qt runtime libraries are installed
    include(Macdeployqt)
    macdeployqt(nitroshare-cli)
endif()
