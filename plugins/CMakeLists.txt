add_subdirectory(broadcast)
add_subdirectory(filesystem)
add_subdirectory(lan)
add_subdirectory(mdns)

if(Qt5Widgets_FOUND)
    add_subdirectory(settingsui)
    add_subdirectory(tray)
endif()

if(BUILD_API)
    add_subdirectory(api)
endif()

if(BUILD_INDICATOR)
    add_subdirectory(indicator)
endif()
