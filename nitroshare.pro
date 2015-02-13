# NitroShare qmake project file
# Copyright 2015 - Nathan Osman

# Ensure that Qt 5.1 is present
lessThan(QT_MAJOR_VERSION, 5) | lessThan(QT_MINOR_VERSION, 1) {
    error("NitroShare requires Qt 5.1 or newer.")
}

# Each of the subdirectories contains its own project file
TEMPLATE = subdirs
SUBDIRS = src
