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
    set(SRC ${SRC} data/resource.rc)
endif()

if(APPLE)
    set(SRC ${SRC}
        data/icon/nitroshare.icns
        transfer/transferwindow.mm
    )
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

# Update the TS files from the source directory
file(GLOB TS data/ts/*.ts)
add_custom_target(ts
    COMMAND "${Qt5_LUPDATE_EXECUTABLE}"
        -silent
        -locations relative
        "${CMAKE_CURRENT_SOURCE_DIR}"
        -ts ${TS}
)

# Create a directory for the compiled QM files
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/data/qm")

# Create commands for each of the translation files
set(QM_XML)
set(QM)
foreach(_ts_path ${TS})
    get_filename_component(_ts_name "${_ts_path}" NAME)
    get_filename_component(_ts_name_we "${_ts_path}" NAME_WE)
    set(_qm_name "${_ts_name_we}.qm")
    set(_qm_path "${CMAKE_CURRENT_BINARY_DIR}/data/qm/${_qm_name}")
    set(QM_XML "${QM_XML}<file>qm/${_qm_name}</file>")
    list(APPEND QM "${_qm_path}")
    add_custom_command(OUTPUT "${_qm_path}"
        COMMAND "${Qt5_LRELEASE_EXECUTABLE}"
            -compress
            -removeidentical
            -silent
            "${_ts_path}"
            -qm "${_qm_path}"
        COMMENT "Generating ${_ts_name}"
    )
endforeach()

# Create a target for compiling all of the translation files
add_custom_target(qm DEPENDS ${QM})

# Configure the i18n resource file
set(QRC_QM "${CMAKE_CURRENT_BINARY_DIR}/data/resource_qm.qrc")
configure_file(data/resource_qm.qrc.in "${QRC_QM}")

qt5_add_resources(QRC
    data/resource.qrc
    "${QRC_QM}"
)

add_executable(nitroshare WIN32 MACOSX_BUNDLE ${SRC} ${UI} ${QRC})

target_compile_features(nitroshare PRIVATE
    cxx_lambdas
    cxx_nullptr
    cxx_strong_enums
    cxx_uniform_initialization
)

# In order to support Retina, a special tag is required in Info.plist.
if(APPLE)
    set_target_properties(nitroshare PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in"
        MACOSX_BUNDLE_ICON_FILE "nitroshare.icns"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.NathanOsman.NitroShare"
        MACOSX_BUNDLE_BUNDLE_NAME "NitroShare"
    )
    target_link_libraries(nitroshare "-framework ApplicationServices")
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

if(WIN32)
    # If windeployqt is available, use it immediately after the build completes to
    # ensure that the dependencies are available.
    include(DeployQt)
    if(WINDEPLOYQT_EXECUTABLE)
        windeployqt(nitroshare)
    endif()

    # If QHttpEngine was used, include it in the installer
    if(QHttpEngine_FOUND)
        add_custom_command(TARGET nitroshare POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E
                copy_if_different "$<TARGET_FILE:QHttpEngine>" "$<TARGET_FILE_DIR:nitroshare>"
            COMMENT "Copying QHttpEngine..."
        )
    endif()

    # If Inno Setup is available, provide a target for building an EXE installer.
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

if(APPLE)
    # If QHttpEngine was used, copy the dynlib over and correct its RPATH
    if(QHttpEngine_FOUND)
        add_custom_command(TARGET nitroshare POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E
                make_directory "$<TARGET_FILE_DIR:nitroshare>/../Frameworks"
            COMMAND "${CMAKE_COMMAND}" -E
                copy_if_different
                    "$<TARGET_FILE:QHttpEngine>"
                    "$<TARGET_FILE_DIR:nitroshare>/../Frameworks/$<TARGET_SONAME_FILE_NAME:QHttpEngine>"
            COMMAND install_name_tool
                -change
                "$<TARGET_SONAME_FILE_NAME:QHttpEngine>"
                "@rpath/$<TARGET_SONAME_FILE_NAME:QHttpEngine>"
                "$<TARGET_FILE:nitroshare>"
            COMMENT "Copying QHttpEngine and correcting RPATH..."
        )
    endif()

    # If macdeployqt is available, use it immediately after the build completes to
    # copy the Qt frameworks into the application bundle.
    include(DeployQt)
    if(MACDEPLOYQT_EXECUTABLE)
        macdeployqt(nitroshare)
    endif()

    # Create a target for building a DMG that contains the application bundle and a
    # symlink to the /Applications folder.
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

# On Linux, include the icons, manpage, .desktop file, and file manager extensions when installing.
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

    # Configure and install Python extensions for Nautilus-derived file managers
    foreach(_file_manager Nautilus Nemo Caja)
        string(TOLOWER ${_file_manager} _file_manager_lower)
        set(_py_filename "${CMAKE_CURRENT_BINARY_DIR}/dist/nitroshare_${_file_manager_lower}.py")
        configure_file("${CMAKE_CURRENT_SOURCE_DIR}/dist/nitroshare.py.in" "${_py_filename}")
        install(FILES "${_py_filename}"
            DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/${_file_manager_lower}-python/extensions"
            RENAME nitroshare.py
        )
    endforeach()

    # Install service files for the Dolphin file manager
    set(KDE_SERVICE_INSTALL_PATH "${CMAKE_INSTALL_DATAROOTDIR}/kservices5"
        CACHE PATH "Path for installation of KDE services"
    )
    install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/dist/nitroshare_addtoservicemenu.desktop"
        DESTINATION "${KDE_SERVICE_INSTALL_PATH}"
    )
    install(PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/dist/nitroshare-send"
        DESTINATION "${CMAKE_INSTALL_BINDIR}"
    )
endif()
