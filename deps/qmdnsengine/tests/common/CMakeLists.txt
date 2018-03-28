set(SRC
    testserver.cpp
    util.cpp
)

add_library(common STATIC ${SRC})
set_target_properties(common PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
)
target_link_libraries(common qmdnsengine)
