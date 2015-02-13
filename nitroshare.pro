# Ensure that Qt 5.1 is present
lessThan(QT_MAJOR_VERSION, 5) | lessThan(QT_MINOR_VERSION, 1) {
    error("NitroShare requires Qt 5.1 or newer.")
}

# Each of the subdirectories contains its own project file
TEMPLATE = subdirs

# Add the directory containing the source code
SUBDIRS  = src

# Add the directory for the current platform
# (The last line is for Linux)
win32:SUBDIRS      += windows
macx:SUBDIRS       += mac
unix:!macx:SUBDIRS += linux
