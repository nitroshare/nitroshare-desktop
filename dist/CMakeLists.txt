# Some of the platforms require additional files to be installed.

if(WIN32)
    find_program(_iscc iscc PATHS
        "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Inno Setup 5_is1;InstallLocation]"
    )
    if(_iscc)
        # Configure the installation script
        if(${CMAKE_SIZEOF_VOID_P} MATCHES "8")
            set(ARCHITECTURE x64)
        endif()
        set(LICENSE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../LICENSE.txt")
        set(LIBRARY_PATH "${CMAKE_CURRENT_BINARY_DIR}/libs")
        configure_file(setup.iss.in "${CMAKE_CURRENT_BINARY_DIR}/setup.iss")

        # Clever hack to find the path to windeployqt
        get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
        get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
        find_program(_windeployqt windeployqt HINTS "${_qt_bin_dir}")

        # CMake offers no method for setting env. variables when invoking
        # commands so this method provides a workaround
        set(_wrapper "${CMAKE_CURRENT_BINARY_DIR}/wrapper.bat")
        file(GENERATE OUTPUT "${_wrapper}" CONTENT
            "@set PATH=${_qt_bin_dir};%PATH%\n@${_windeployqt} %*"
        )

        # Create the runtime target for copying the Qt libraries
        add_custom_target(runtime
            ${_wrapper}
                --dir "${LIBRARY_PATH}"
                --no-compiler-runtime
                "$<TARGET_FILE:nitroshare>"
            COMMENT "Copying Qt runtime..."
        )

        # Sometimes windeployqt doesn't properly copy MSVC++ runtime libraries
        # so we fall back on the built-in CMake module to do this
        include(InstallRequiredSystemLibraries)
        foreach(lib ${CMAKE_INSTALL_SYSTEM_RUNTIME_LIBS})
            get_filename_component(_filename "${lib}" NAME)
            add_custom_command(TARGET runtime
                COMMAND ${CMAKE_COMMAND} -E copy_if_different "${lib}" "${LIBRARY_PATH}"
                COMMENT "Copying ${_filename}..."
            )
        endforeach()

        # Create the target that builds the installer
        add_custom_target(exe
            ${_iscc}
                /Q
                /DTARGET_FILE="$<TARGET_FILE:nitroshare>"
                /DTARGET_FILE_NAME="$<TARGET_FILE_NAME:nitroshare>"
                "${CMAKE_CURRENT_BINARY_DIR}/setup.iss"
            DEPENDS runtime nitroshare
            COMMENT "Building installer..."
        )
    endif()
endif()

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
