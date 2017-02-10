file(GLOB HEADERS include/nitroshare-common/*.h)

set(SRC
    src/init.cpp
    src/signal.cpp
)

add_library(nitroshare-common STATIC ${HEADERS} ${SRC})

set_target_properties(nitroshare-common PROPERTIES
    CXX_STANDARD  11
)

target_include_directories(nitroshare PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
)

target_link_libraries(nitroshare-common nitroshare Qt5::Core)
