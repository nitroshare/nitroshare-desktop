set(SRC
    dummy2plugin.h
    dummy2plugin.cpp
)

add_library(dummy2 MODULE ${SRC})

set_target_properties(dummy2 PROPERTIES
    CXX_STANDARD  11
)
target_link_libraries(dummy2 nitroshare)
