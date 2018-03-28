add_subdirectory(common)

set(TESTS
    TestBrowser
    TestCache
    TestDns
    TestHostname
    TestProber
    TestProvider
    TestResolver
)

foreach(_test ${TESTS})
    add_executable(${_test} ${_test}.cpp)
    set_target_properties(${_test} PROPERTIES
        CXX_STANDARD 11
        CXX_STANDARD_REQUIRED ON
    )
    target_include_directories(${_test} PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")
    target_link_libraries(${_test} qmdnsengine Qt5::Test common)
    add_test(NAME ${_test}
        COMMAND ${_test}
    )
endforeach()

# On Windows, the tests will not run without the DLL located in the current
# directory - a target must be used to copy it here once built
if(WIN32)
    add_custom_target(qmdnsengine-copy ALL
        "${CMAKE_COMMAND}" -E copy_if_different \"$<TARGET_FILE:qmdnsengine>\" \"${CMAKE_CURRENT_BINARY_DIR}\"
        DEPENDS qmdnsengine
    )
endif()
