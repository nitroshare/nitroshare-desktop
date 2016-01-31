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

qt5_use_modules(nitroshare Widgets Network Svg)
if(Qt5WinExtras_FOUND)
    qt5_use_modules(nitroshare WinExtras)
endif()
if(Qt5MacExtras_FOUND)
    qt5_use_modules(nitroshare MacExtras)
endif()

if(APPINDICATOR_FOUND)
    target_include_directories(nitroshare PRIVATE ${APPINDICATOR_INCLUDE_DIRS})
    target_link_libraries(nitroshare ${APPINDICATOR_LIBRARIES})
endif()

if(UNITY_FOUND)
    target_include_directories(nitroshare PRIVATE ${UNITY_INCLUDE_DIRS})
    target_link_libraries(nitroshare ${UNITY_LIBRARIES})
endif()

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
        configure_file(setup.iss.in "${CMAKE_CURRENT_BINARY_DIR}/setup.iss")
        add_custom_target(exe
            "${INNOSETUP_EXECUTABLE}"
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
if(APPLE)
    set_target_properties(nitroshare PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in"
        MACOSX_BUNDLE_ICON_FILE "nitroshare.icns"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.NathanOsman.NitroShare"
        MACOSX_BUNDLE_BUNDLE_NAME "NitroShare"
    )
    include(DeployQt)
    if(MACDEPLOYQT_EXECUTABLE)
        macdeployqt(nitroshare)
    endif()
endif()

install(TARGETS nitroshare
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    BUNDLE DESTINATION .
)
