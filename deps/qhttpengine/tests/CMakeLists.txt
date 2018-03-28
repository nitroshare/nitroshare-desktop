add_subdirectory(common)

set(TESTS
    TestBasicAuthMiddleware
    TestFilesystemHandler
    TestHandler
    TestIByteArray
    TestLocalAuthMiddleware
    TestLocalFile
    TestMiddleware
    TestParser
    TestProxyHandler
    TestQIODeviceCopier
    TestQObjectHandler
    TestRange
    TestServer
    TestSocket
)

qt5_add_resources(QRC resource.qrc)

foreach(TEST ${TESTS})
    add_executable(${TEST} ${TEST}.cpp ${QRC})
    set_target_properties(${TEST} PROPERTIES
        CXX_STANDARD 11
        CXX_STANDARD_REQUIRED ON
    )
    target_include_directories(${TEST} PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")
    target_link_libraries(${TEST} Qt5::Test qhttpengine common)
    add_test(NAME ${TEST}
        COMMAND ${TEST}
    )
endforeach()

# On Windows, the library's DLL must exist in the same directory as the test
# executables which link against it - create a custom command to copy it
if(WIN32 AND NOT BUILD_STATIC)
    add_custom_target(qhttpengine-copy ALL
        "${CMAKE_COMMAND}" -E copy_if_different \"$<TARGET_FILE:qhttpengine>\" \"${CMAKE_CURRENT_BINARY_DIR}\"
        DEPENDS qhttpengine
    )
endif()
