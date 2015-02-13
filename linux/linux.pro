include(../nitroshare.pri)

# This project doesn't build anything
TEMPLATE = aux

# Ensure that the .desktop and .svg files are copied to the appropriate location
desktop_file.path  = $${PREFIX}/share/applications
desktop_file.files = nitroshare.desktop
icon_file.path     = $${PREFIX}/share/pixmaps
icon_file.files    = nitroshare.svg

# Add these to the install target
INSTALLS += \
    desktop_file \
    icon_file
