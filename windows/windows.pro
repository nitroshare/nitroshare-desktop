include(../nitroshare.pri)

# This project doesn't build anything
TEMPLATE = aux

# Determine whether the build is for win32 or win64
!contains(QMAKE_TARGET.arch, x86_64) {
    ARCH = 32
} else {
    ARCH = 64
}

# Define the filename of the main executable and installer
EXE_FILENAME       = $${PROJECT_NAME}.exe
INSTALLER_FILENAME = $${PROJECT_NAME}-$${PROJECT_VERSION}-win$${ARCH}.exe

# Generate the setup.iss setup script
setup_iss.input    = setup.iss.in
setup_iss.output   = $${OUT}/setup.iss
QMAKE_SUBSTITUTES += setup_iss

# Target for gathering the required Qt libraries
windeployqt.commands = windeployqt $${DESTDIR}/$${EXE_FILENAME}

# Target for generating the Windows installer (depends on the Qt libraries)
exe_installer.commands = iscc setup.iss
exe_installer.depends  = windeployqt

# Add the two extra targets
QMAKE_EXTRA_TARGETS += \
    windeployqt \
    exe_installer
