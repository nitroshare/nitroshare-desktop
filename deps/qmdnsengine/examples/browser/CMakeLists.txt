set(SRC
    browser.cpp
    mainwindow.cpp
    servicemodel.cpp
)

add_executable(browser WIN32 ${SRC})

set_target_properties(browser PROPERTIES
    CXX_STANDARD 11
)

target_link_libraries(browser qmdnsengine Qt5::Widgets)

install(TARGETS browser
    RUNTIME DESTINATION "${EXAMPLE_DIR}"
    COMPONENT examples
)
