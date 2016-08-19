add_subdirectory(broadcast)
add_subdirectory(mdns)

if(BUILD_API)
    add_subdirectory(api)
endif()
