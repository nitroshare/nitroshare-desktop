# Ensure that Qt 5.1 is present
lessThan(QT_MAJOR_VERSION, 5) | lessThan(QT_MINOR_VERSION, 1) {
    error("NitroShare requires Qt 5.1 or newer.")
}

# Each of the subdirectories contains its own project file
TEMPLATE = subdirs

# Add the directory containing the source code
SUBDIRS  = src

# Add the directory and targets for the current platform
win32 {
    SUBDIRS             += windows
    exe.target           = exe
    exe.CONFIG           = recursive
    QMAKE_EXTRA_TARGETS += exe
}

macx {
    SUBDIRS += mac
}

linux-* {
    SUBDIRS += linux
}
