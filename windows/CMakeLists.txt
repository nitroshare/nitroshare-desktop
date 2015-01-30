# Add the resource file to the build
configure_file(resource.rc.in "${CMAKE_CURRENT_BINARY_DIR}/resource.rc")
set(SRC ${SRC} resource.rc)

# Include required system libraries when installing
include(InstallRequiredSystemLibraries)
