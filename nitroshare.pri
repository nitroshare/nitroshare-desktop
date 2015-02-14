# Define a few important variables that describe the application
PROJECT_NAME        = nitroshare
PROJECT_TITLE       = NitroShare
PROJECT_DESCRIPTION = Network File Transfer Application
PROJECT_AUTHOR      = Nathan Osman
PROJECT_DOMAIN      = nitroshare.co
PROJECT_WEBSITE     = http://$${PROJECT_DOMAIN}

# Define the components of the application version
PROJECT_VERSION_MAJOR = 0
PROJECT_VERSION_MINOR = 3
PROJECT_VERSION_PATCH = 0

# Define a string that combines the application version components
PROJECT_VERSION = $${PROJECT_VERSION_MAJOR}.$${PROJECT_VERSION_MINOR}.$${PROJECT_VERSION_PATCH}

# Don't pollute the source tree with files everywhere!
# Instead, define filetype-specific directories where built files should go
OUT         = $${PWD}/out
DESTDIR     = $${OUT}/install
INCLUDE_DIR = $${OUT}/include
MOC_DIR     = $${OUT}/moc
OBJECTS_DIR = $${OUT}/obj
RCC_DIR     = $${OUT}/rcc
UI_DIR      = $${OUT}/ui
