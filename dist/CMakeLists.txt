if(WIN32)
    # Prepare the InnoSetup configuration script
    configure_file(setup.iss.in "${CMAKE_CURRENT_BINARY_DIR}/setup.iss")

    # The MSVC 2015 runtime requires CMake 3.6+
    if((MSVC_VERSION VERSION_EQUAL 1900 OR MSVC_VERSION VERSION_GREATER 1900)
            AND CMAKE_VERSION VERSION_LESS "3.6")
        message(WARNING "MSVC 2015+ runtime requires CMake 3.6+")
    endif()

    # Ensure the C/C++ runtime is installed
    set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)
    include(InstallRequiredSystemLibraries)
endif()

if(APPLE)
    # Prepare the plist file for the app bundle
    configure_file(Info.plist.in "${CONTENTS_DIRECTORY}/Info.plist")

    # Copy the icon to the Resources directory
    file(COPY nitroshare.icns DESTINATION "${CONTENTS_DIRECTORY}/Resources")

    # Add a target for building the DMG
    set(sym "${OUTPUT_PREFIX}/Applications")
    set(dmg "${CMAKE_BINARY_DIR}/nitroshare-${PROJECT_VERSION}-osx.dmg")
    add_custom_target(dmg
        COMMAND rm -f \"${sym}\" \"${dmg}\"
        COMMAND ln -s /Applications \"${sym}\"
        COMMAND hdiutil create
            -srcfolder \"${OUTPUT_PREFIX}\"
            -volname "${PROJECT_NAME}"
            -fs HFS+
            -size 30m
            \"${dmg}\"
        DEPENDS nitroshare
        COMMENT "Building disk image..."
    )
endif()

if(LINUX AND BUILD_UI)
    # Copy the application icons
    install(DIRECTORY icons
        DESTINATION share
    )

    # Copy the .desktop file
    install(FILES nitroshare.desktop
        DESTINATION share/applications
    )
endif()
