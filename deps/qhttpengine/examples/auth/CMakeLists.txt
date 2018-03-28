# Client
add_executable(authclient client.cpp)
target_link_libraries(authclient qhttpengine)
set_target_properties(authclient PROPERTIES
    CXX_STANDARD 11
)
install(TARGETS authclient
    RUNTIME DESTINATION "${EXAMPLE_DIR}"
    COMPONENT examples
)

# Server
add_executable(authserver server.cpp)
target_link_libraries(authserver qhttpengine)
set_target_properties(authserver PROPERTIES
    CXX_STANDARD 11
)
install(TARGETS authserver
    RUNTIME DESTINATION "${EXAMPLE_DIR}"
    COMPONENT examples
)
