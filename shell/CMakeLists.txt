set(SRC
    classfactory.h
    classfactory.cpp
    contextmenu.h
    contextmenu.cpp
    nitroshell.cpp
    nitroshell.def
    registry.h
    registry.cpp
)

add_library(nitroshell SHARED ${SRC})

target_link_libraries(nitroshell nitroshare Qt5::Core)

install(TARGETS nitroshell
    RUNTIME DESTINATION ${INSTALL_BIN_PATH}
)
