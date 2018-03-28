set(SRC
    apihandler.cpp
    main.cpp
)

qt5_add_resources(QRC resources.qrc)

add_executable(chatserver ${SRC} ${QRC})
target_link_libraries(chatserver qhttpengine)

install(TARGETS chatserver
    RUNTIME DESTINATION "${EXAMPLE_DIR}"
    COMPONENT examples
)
