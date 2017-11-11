set(SRC
    mockdevice.h
    mockdevice.cpp
    mockhandler.h
    mockhandler.cpp
    mockitem.h
    mockitem.cpp
    mocktransport.h
    mocktransport.cpp
    mocktransportserver.h
    mocktransportserver.cpp
)

add_library(mock SHARED ${SRC})

set_target_properties(mock PROPERTIES
    CXX_STANDARD 11
)

target_link_libraries(mock nitroshare)
