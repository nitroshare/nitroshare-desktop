# NitroShare qmake project file
# Copyright 2015 - Nathan Osman

# Define the components of the application version
NITROSHARE_VERSION_MAJOR = 0
NITROSHARE_VERSION_MINOR = 3
NITROSHARE_VERSION_PATCH = 0

# Create a string that combines the application version
NITROSHARE_VERSION = $${NITROSHARE_VERSION_MAJOR}.$${NITROSHARE_VERSION_MINOR}.$${NITROSHARE_VERSION_PATCH}

# Add the version components to the list of defines
DEFINES += \
    NITROSHARE_VERSION_MAJOR=$${NITROSHARE_VERSION_MAJOR} \
    NITROSHARE_VERSION_MINOR=$${NITROSHARE_VERSION_MINOR} \
    NITROSHARE_VERSION_PATCH=$${NITROSHARE_VERSION_PATCH} \
    NITROSHARE_VERSION=\\\"$${NITROSHARE_VERSION}\\\"

# Don't pollute the source tree with files everywhere!
# Instead, create a directory to store the files
DESTDIR     = $${PWD}/out
OBJECTS_DIR = $${PWD}/out/obj
MOC_DIR     = $${PWD}/out/moc
RCC_DIR     = $${PWD}/out/rcc
UI_DIR      = $${PWD}/out/ui
