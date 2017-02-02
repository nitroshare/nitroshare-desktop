set(SRC
    signal.cpp
)

add_library(nitroshare-common STATIC ${SRC})

set_target_properties(nitroshare-common PROPERTIES
    CXX_STANDARD 11
)

target_link_libraries(nitroshare-common Qt5::Core)
