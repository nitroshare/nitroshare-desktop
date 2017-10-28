set(SRC
    dummy2plugin.h
    dummy2plugin.cpp
)

add_library(dummy2 MODULE ${SRC})

set_target_properties(dummy2 PROPERTIES
    CXX_STANDARD             11
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/../plugins"
)
target_link_libraries(dummy2 nitroshare)
