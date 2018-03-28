set(SRC
    main.cpp
)

add_executable(fileserver ${SRC})
target_link_libraries(fileserver qhttpengine)

install(TARGETS fileserver
    RUNTIME DESTINATION "${EXAMPLE_DIR}"
    COMPONENT examples
)
