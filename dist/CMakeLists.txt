# Some of the platforms require additional files to be installed.

# Linux requires icons and a .desktop file.
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/icons"
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}"
    )
    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/nitroshare.1"
        DESTINATION "${CMAKE_INSTALL_MANDIR}/man1"
    )
    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/nitroshare.desktop"
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/applications"
    )
endif()
