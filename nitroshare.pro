include(nitroshare.pri)

# The project currently consists of a single subdir
# (Soon this will be expanded as functionality is broken up)
TEMPLATE   = subdirs
SUBDIRS    = src
src.target = src

OTHER_FILES += \
    dist/setup.iss.in \
    LICENSE.txt \
    README.md

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
    INSTALLER_NAME     = $${PROJECT_NAME}-$${PROJECT_VERSION}-win$${ARCH}
    INSTALLER_FILENAME = $${INSTALLER_NAME}.exe

    # Generate the setup.iss setup script
    setup_iss.input    = dist/setup.iss.in
    setup_iss.output   = $${OUT}/setup.iss
    QMAKE_SUBSTITUTES += setup_iss

    # Target for copying the required Qt libraries to the installation directory
    # Note: no target is needed since the command is idempotent and fairly quick
    qtlibs.commands      = windeployqt $${DESTDIR}/$${EXE_FILENAME}
    qtlibs.depends       = src
    QMAKE_EXTRA_TARGETS += qtlibs

    # Target for creating the installer
    installer.target     = $${OUT}/$${INSTALLER_FILENAME}
    installer.commands   = iscc $${OUT}/setup.iss
    installer.depends    = qtlibs
    QMAKE_EXTRA_TARGETS += installer

    # Alias target for building the installer
    exe.depends          = installer
    QMAKE_EXTRA_TARGETS += exe
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
    appsymlink.commands  = \
        rm -f $${DESTDIR}/Applications; \
        ln -s /Applications $${DESTDIR}/Applications
    appsymlink.depends   = src
    QMAKE_EXTRA_TARGETS += appsymlink

    # Target for creating the disk image
    image.target         = $${OUT}/$${IMAGE_FILENAME}
    image.commands       = \
        test -e $${OUT}/$${IMAGE_FILENAME} && rm $${OUT}/$${IMAGE_FILENAME}; \
        hdiutil create -srcfolder $${DESTDIR} -volname $${PROJECT_TITLE} -fs HFS+ -size 30m $${OUT}/$${IMAGE_FILENAME}
    image.depends        = qtlibs appsymlink
    QMAKE_EXTRA_TARGETS += image

    # Alias target for building the DMG
    dmg.depends          = image
    QMAKE_EXTRA_TARGETS += dmg
}

# Add files that are specific to the Linux build
linux-* {
    desktop_file.path  = $${PREFIX}/share/applications
    desktop_file.files = dist/nitroshare.desktop
    icons.path         = $${PREFIX}/share/icons
    icons.files        = dist/icons/*
    INSTALLS          += desktop_file icons
}
