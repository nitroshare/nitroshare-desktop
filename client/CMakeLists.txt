set(SRC
    main.cpp
)

add_executable(nitroshare-client ${SRC})

set_target_properties(nitroshare-cli PROPERTIES
    CXX_STANDARD 11
    OUTPUT_NAME  nitroshare
)

target_link_libraries(nitroshare-client Qt5::Core)

install(TARGETS nitroshare-client
    DESTINATION "${INSTALL_BIN_PATH}"
)
