# Determine the path to windeployqt by finding the path to qmake
get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
find_program(_windeployqt windeployqt HINTS "${_qt_bin_dir}")

# Add a custom target that installs the Qt libraries
add_custom_target(install_qt_libraries
    ${_windeployqt} --dir "${CMAKE_INSTALL_PREFIX}/bin" "$<TARGET_FILE:nitroshare>")
add_dependencies(install_qt_libraries nitroshare)

# Add the system libraries to the install target
include(InstallRequiredSystemLibraries)

