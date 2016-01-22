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

if(APPINDICATOR_SUPPORT)
    set(SRC ${SRC}
        icon/indicatoricon.cpp
    )
endif()

if(WIN32)
    configure_file(data/resource.rc.in "${CMAKE_CURRENT_BINARY_DIR}/resource.rc")
    set(SRC ${SRC} "${CMAKE_CURRENT_BINARY_DIR}/resource.rc")
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

add_executable(nitroshare WIN32 ${SRC} ${UI} ${QRC})

# Since Qt and GTK+ both define the symbol "signals", it is necessary to
# ensure that it is not defined by Qt.
add_definitions(-DQT_NO_SIGNALS_SLOTS_KEYWORDS)

qt5_use_modules(nitroshare Widgets Network Svg)

target_compile_features(nitroshare PRIVATE
    cxx_lambdas
    cxx_nullptr
    cxx_strong_enums
    cxx_uniform_initialization
)

if(APPINDICATOR_SUPPORT)
    target_include_directories(nitroshare PRIVATE ${APPINDICATOR_INCLUDE_DIRS})
    target_link_libraries(nitroshare ${APPINDICATOR_LIBRARIES})
endif()

if(UNITY_INTEGRATION)
    target_include_directories(nitroshare PRIVATE ${UNITY_INCLUDE_DIRS})
    target_link_libraries(nitroshare ${UNITY_LIBRARIES})
endif()

install(TARGETS nitroshare
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
