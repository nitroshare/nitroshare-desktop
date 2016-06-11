configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/config.h")

set(SRC
    application/aboutdialog.cpp
    application/application.cpp
    application/splashdialog.cpp
    bundle/bundle.cpp
    device/device.cpp
    device/devicedialog.cpp
    device/devicelistener.cpp
    device/devicemodel.cpp
    icon/icon.cpp
    icon/trayicon.cpp
    settings/settings.cpp
    settings/settingsdialog.cpp
    transfer/transfer.cpp
    transfer/transfermodel.cpp
    transfer/transferreceiver.cpp
    transfer/transfersender.cpp
    transfer/transferserver.cpp
    transfer/transferwindow.cpp
    util/json.cpp
    util/platform.cpp
    main.cpp
)

if(WIN32)
    set(SRC ${SRC} "data/resource.rc")
endif()

if(APPLE)
    set(SRC ${SRC} "data/icon/nitroshare.icns")
    set_source_files_properties("data/icon/nitroshare.icns" PROPERTIES
        MACOSX_PACKAGE_LOCATION Resources
    )
endif()

if(QHttpEngine_FOUND)
    set(SRC ${SRC}
        api/apihandler.cpp
        api/apiserver.cpp
    )
endif()

if(APPINDICATOR_FOUND)
    set(SRC ${SRC}
        icon/indicatoricon.cpp
    )
endif()

qt5_wrap_ui(UI
    application/aboutdialog.ui
    application/splashdialog.ui
    device/devicedialog.ui
    settings/settingsdialog.ui
    transfer/transferwindow.ui
)

qt5_add_resources(QRC
    data/resource.qrc
)

add_executable(nitroshare WIN32 MACOSX_BUNDLE ${SRC} ${UI} ${QRC})

target_compile_features(nitroshare PRIVATE
    cxx_lambdas
    cxx_nullptr
    cxx_strong_enums
    cxx_uniform_initialization
)

# By default, Visual C++ produces PE files with "minimum OS Required" set to
# 6.0 (Windows Vista). A special linker flag is needed for XP support.
if(WIN32 AND MSVC)
    if(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
        set_target_properties(nitroshare PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS,5.02")
    else()
        set_target_properties(nitroshare PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS,5.01")
    endif()
endif()

# In order to support Retina, a special tag is required in Info.plist.
if(APPLE)
    set_target_properties(nitroshare PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in"
        MACOSX_BUNDLE_ICON_FILE "nitroshare.icns"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.NathanOsman.NitroShare"
        MACOSX_BUNDLE_BUNDLE_NAME "NitroShare"
    )
endif()

# GCC decided to be clever and ignore unused shared libraries during linking
# making this hack necessary to ensure QtSVG is linked against
if(CMAKE_COMPILER_IS_GNUCXX)
    target_link_libraries(nitroshare -Wl,--no-as-needed)
endif()

target_link_libraries(nitroshare Qt5::Widgets Qt5::Network Qt5::Svg)
if(Qt5WinExtras_FOUND)
    target_link_libraries(nitroshare Qt5::WinExtras)
endif()
if(Qt5MacExtras_FOUND)
    target_link_libraries(nitroshare Qt5::MacExtras)
endif()

if(QHttpEngine_FOUND)
    target_link_libraries(nitroshare QHttpEngine)
endif()

if(APPINDICATOR_FOUND)
    target_include_directories(nitroshare PRIVATE ${APPINDICATOR_INCLUDE_DIRS})
    target_link_libraries(nitroshare ${APPINDICATOR_LIBRARIES})
endif()

include(GNUInstallDirs)

install(TARGETS nitroshare
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    BUNDLE DESTINATION .
)

# If windeployqt is available, use it immediately after the build completes to
# ensure that the dependencies are available.
# If Inno Setup is available, provide a target for building an EXE installer.
if(WIN32)
    include(DeployQt)
    if(WINDEPLOYQT_EXECUTABLE)
        windeployqt(nitroshare)
    endif()
    find_package(InnoSetup)
    if(INNOSETUP_EXECUTABLE)
        configure_file(dist/setup.iss.in "${CMAKE_CURRENT_BINARY_DIR}/setup.iss")
        add_custom_target(exe
            COMMAND "${INNOSETUP_EXECUTABLE}"
                /Q
                /DTARGET_FILE_NAME="$<TARGET_FILE_NAME:nitroshare>"
                "${CMAKE_CURRENT_BINARY_DIR}/setup.iss"
            DEPENDS nitroshare
            COMMENT "Building installer..."
        )
    endif()
endif()

# If macdeployqt is available, use it immediately after the build completes to
# copy the Qt frameworks into the application bundle.
# Create a target for building a DMG that contains the application bundle and a
# symlink to the /Applications folder.
if(APPLE)
    include(DeployQt)
    if(MACDEPLOYQT_EXECUTABLE)
        macdeployqt(nitroshare)
    endif()
    set(sym "${CMAKE_BINARY_DIR}/out/Applications")
    set(dmg "${CMAKE_BINARY_DIR}/nitroshare-${PROJECT_VERSION}-osx.dmg")
    add_custom_target(dmg
        COMMAND rm -f "${sym}" "${dmg}"
        COMMAND ln -s /Applications "${sym}"
        COMMAND hdiutil create
            -srcfolder "${CMAKE_BINARY_DIR}/out"
            -volname "${PROJECT_NAME}"
            -fs HFS+
            -size 30m
            "${dmg}"
        DEPENDS nitroshare
        COMMENT "Building disk image..."
    )
endif()

# On Linux, include the icons, manpage, and .desktop file when installing.
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/dist/icons"
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}"
    )
    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/dist/nitroshare.1"
        DESTINATION "${CMAKE_INSTALL_MANDIR}/man1"
    )
    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/dist/nitroshare.desktop"
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/applications"
    )
endif()
