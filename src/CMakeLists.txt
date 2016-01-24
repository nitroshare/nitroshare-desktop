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

if(APPINDICATOR_FOUND)
    set(SRC ${SRC}
        icon/indicatoricon.cpp
    )
endif()

if(WIN32)
    set(SRC ${SRC} "data/resource.rc")
endif()

if(APPLE)
    set_source_files_properties("data/icon/nitroshare.icns" PROPERTIES
        MACOSX_PACKAGE_LOCATION Resources
    )
    set(SRC ${SRC} "data/icon/nitroshare.icns")
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

# Since Qt and GTK+ both define the symbol "signals", it is necessary to
# ensure that it is not defined by Qt.
add_definitions(-DQT_NO_SIGNALS_SLOTS_KEYWORDS)

qt5_use_modules(nitroshare Widgets Network Svg)
if(Qt5WinExtras_FOUND)
    qt5_use_modules(nitroshare WinExtras)
endif()
if(Qt5MacExtras_FOUND)
    qt5_use_modules(nitroshare MacExtras)
endif()

target_compile_features(nitroshare PRIVATE
    cxx_lambdas
    cxx_nullptr
    cxx_strong_enums
    cxx_uniform_initialization
)

if(APPINDICATOR_FOUND)
    target_include_directories(nitroshare PRIVATE ${APPINDICATOR_INCLUDE_DIRS})
    target_link_libraries(nitroshare ${APPINDICATOR_LIBRARIES})
endif()

if(UNITY_FOUND)
    target_include_directories(nitroshare PRIVATE ${UNITY_INCLUDE_DIRS})
    target_link_libraries(nitroshare ${UNITY_LIBRARIES})
endif()

if(APPLE)
    set_target_properties(nitroshare PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in"
        MACOSX_BUNDLE_ICON_FILE "nitroshare.icns"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.NathanOsman.NitroShare"
        MACOSX_BUNDLE_BUNDLE_NAME "NitroShare"
    )

    # Clever hack to find the path to macdeployqt
    get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
    find_program(_macdeployqt macdeployqt HINTS "${_qt_bin_dir}")

    # Run macdeployqt after building
    add_custom_command(TARGET nitroshare POST_BUILD
        COMMAND ${_macdeployqt} "$<TARGET_FILE_DIR:nitroshare>/../.." -always-overwrite
        COMMENT "Running macdeployqt..."
    )
endif()

install(TARGETS nitroshare
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    BUNDLE DESTINATION .
)
