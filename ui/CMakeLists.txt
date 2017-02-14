set(SRC
    main.cpp
)

add_executable(nitroshare-ui ${SRC})

set_target_properties(nitroshare-ui PROPERTIES
    CXX_STANDARD 11
)

target_include_directories(nitroshare-ui PUBLIC
    "${CMAKE_CURRENT_BINARY_DIR}"
)

target_link_libraries(nitroshare-ui nitroshare Qt5::Core Qt5::Widgets)

install(TARGETS nitroshare-ui
    DESTINATION "${INSTALL_BIN_PATH}"
)
