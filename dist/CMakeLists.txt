# Some of the platforms require additional files to be installed.

# Linux requires icons and a .desktop file.
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/icons"
        DESTINATION share/icons
    )
    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/nitroshare.desktop"
        DESTINATION share/applications
    )
endif()
