set(SRC
    classfactory.cpp
    contextmenu.cpp
    nitroshell.cpp
    nitroshell.def
    registry.cpp
)

add_library(nitroshell SHARED ${SRC})

install(TARGETS nitroshell
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
