add_subdirectory(broadcast)
add_subdirectory(filesystem)
add_subdirectory(lan)

if(Qt5Widgets_FOUND)
    add_subdirectory(deviceui)
    add_subdirectory(filesystemui)
    add_subdirectory(loggerui)
    add_subdirectory(settingsui)
    add_subdirectory(tray)
endif()

if(BUILD_API)
    add_subdirectory(api)
endif()

if(BUILD_INDICATOR)
    add_subdirectory(indicator)
endif()

if(BUILD_MDNS)
    add_subdirectory(mdns)
endif()
