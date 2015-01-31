# If CMAKE_BUILD_TYPE is not set, we must set it to "Release" in order to
# avoid errors when linking against the Qt libraries
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# TODO: use windeployqt to install Qt libraries
# windeployqt --dry-run --list mapping nitroshare.exe

# Include required system libraries when installing
include(InstallRequiredSystemLibraries)
