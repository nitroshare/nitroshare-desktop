if(WIN32 AND INNOSETUP_EXECUTABLE)
    # Prepare the InnoSetup configuration script
    configure_file(setup.iss.in "${CMAKE_CURRENT_BINARY_DIR}/setup.iss")

    # Add the target for building the .exe
    add_custom_target(exe
        COMMAND "${INNOSETUP_EXECUTABLE}"
            /Q
            /DTARGET_FILE_NAME="$<TARGET_FILE_NAME:nitroshare-ui>"
            \"${CMAKE_CURRENT_BINARY_DIR}/setup.iss\"
        COMMENT "Building installer..."
    )
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
    # Copy the application icons to the correct location
    install(DIRECTORY icons
        DESTINATION share
    )
endif()
