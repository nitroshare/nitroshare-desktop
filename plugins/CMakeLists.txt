add_subdirectory(broadcast)
add_subdirectory(filesystem)
add_subdirectory(lan)
add_subdirectory(mdns)

if(BUILD_API)
    add_subdirectory(api)
endif()

if(BUILD_INDICATOR)
    add_subdirectory(indicator)
endif()
