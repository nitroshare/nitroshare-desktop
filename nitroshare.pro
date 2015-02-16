include(nitroshare.pri)

# The project currently consists of a single subdir
# (Soon this will be expanded as functionality is broken up)
TEMPLATE   = subdirs
SUBDIRS    = src
src.target = src

# Add targets that are specific to the Windows build
win32 {
    # Determine whether the build is for win32 or win64
    !contains(QMAKE_TARGET.arch, x86_64) {
        ARCH = 32
    } else {
        ARCH = 64
    }

    # Define the filename of the main executable and installer
    EXE_FILENAME       = $${PROJECT_NAME}.exe
    INSTALLER_FILENAME = $${PROJECT_NAME}-$${PROJECT_VERSION}-win$${ARCH}

    # Generate the setup.iss setup script
    setup_iss.input    = dist/setup.iss.in
    setup_iss.output   = $${OUT}/setup.iss
    QMAKE_SUBSTITUTES += setup_iss

    # Targets for gathering the required Qt libraries and building the exe
    qtlibs.commands      = windeployqt $${DESTDIR}/$${EXE_FILENAME}
    qtlibs.depends       = src
    exe.commands         = iscc $${OUT}/setup.iss
    exe.depends          = qtlibs
    QMAKE_EXTRA_TARGETS += qtlibs exe
}

# Add targets that are specific to the Mac build
macx {
    # Define the name of the bundle and disk image produced
    BUNDLE_FILENAME = $${PROJECT_NAME}.app
    IMAGE_FILENAME  = $${PROJECT_NAME}-$${PROJECT_VERSION}-osx.dmg

    # Target for copying the required Qt frameworks to the bundle
    qtlibs.target        = $${DESTDIR}/$${BUNDLE_FILENAME}/Contents/Frameworks
    qtlibs.commands      = macdeployqt $${DESTDIR}/$${BUNDLE_FILENAME}
    qtlibs.depends       = src
    QMAKE_EXTRA_TARGETS += qtlibs

    # Target for creating the symlink to /Applications
    appsymlink.target    = $${DESTDIR}/Applications
    appsymlink.commands  = ln -s /Applications $${DESTDIR}/Applications
    QMAKE_EXTRA_TARGETS += appsymlink
    QMAKE_CLEAN         += $${DESTDIR}/Applications

    # Target for creating the disk image
    image.target         = $${OUT}/$${IMAGE_FILENAME}
    image.commands       = hdiutil create -srcfolder $${DESTDIR} -volname $${PROJECT_TITLE} -fs HFS+ -size 30m $${OUT}/$${IMAGE_FILENAME}
    image.depends        = qtlibs appsymlink
    QMAKE_EXTRA_TARGETS += image
    QMAKE_CLEAN         += $${OUT}/$${IMAGE_FILENAME}

    # Alias target for building the DMG
    dmg.depends          = image
    QMAKE_EXTRA_TARGETS += dmg
}

# Add files that are specific to the Linux build
linux-* {
    desktop_file.path  = $${PREFIX}/share/applications
    desktop_file.files = dist/nitroshare.desktop
    icon_file.path     = $${PREFIX}/share/pixmaps
    icon_file.files    = dist/nitroshare.svg
    INSTALLS          += desktop_file icon_file
}
