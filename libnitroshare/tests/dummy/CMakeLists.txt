set(SRC
    dummyplugin.h
    dummyplugin.cpp
)

add_library(dummy MODULE ${SRC})

set_target_properties(dummy PROPERTIES
    CXX_STANDARD  11
)
target_link_libraries(dummy nitroshare)
