set(TESTS
    TestDeviceModel
)

foreach(_test ${TESTS})
    add_executable(${_test} ${_test}.cpp)
    set_target_properties(${_test} PROPERTIES
        CXX_STANDARD  11
    )
    target_link_libraries(${_test} nitroshare Qt5::Test)
    add_test(NAME ${_test}
        COMMAND ${_test}
    )
endforeach()
