set(SRC
    main.cpp
)

add_executable(nitroshare-nmh WIN32 ${SRC})

set_target_properties(nitroshare-nmh PROPERTIES
    CXX_STANDARD 11
)

target_link_libraries(nitroshare-nmh nitroshare Qt5::Core)

install(TARGETS nitroshare-nmh
    DESTINATION "${INSTALL_BIN_PATH}"
)

if(WIN32)
    windeployqt(nitroshare-nmh)
    if(INNOSETUP_EXECUTABLE)
        add_dependencies(exe nitroshare-nmh)
    endif()
endif()
