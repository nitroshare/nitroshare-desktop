set(SRC
    qsimplehttpclient.cpp
    qsocketpair.cpp
)

add_library(common STATIC ${SRC})
set_target_properties(common PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
)
target_link_libraries(common Qt5::Network qhttpengine)
