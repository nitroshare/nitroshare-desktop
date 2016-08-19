set(SRC
    main.cpp
)

add_executable(nitroshare-daemon ${SRC})

set_target_properties(nitroshare-daemon PROPERTIES
    CXX_STANDARD 11
    OUTPUT_NAME nitroshare
)

target_link_libraries(nitroshare-daemon nitroshare Qt5::Core)

install(TARGETS nitroshare-daemon
    DESTINATION bin
)
