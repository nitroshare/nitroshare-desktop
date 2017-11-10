# Due to the large amount of classes that need to be mocked, a separate library
# is used for convenience; two "dummy" plugins are provided as well
add_subdirectory(mock)
add_subdirectory(dummy)
add_subdirectory(dummy2)

set(TESTS
    TestDeviceModel
    TestFileUtil
    TestJsonUtil
    TestLogger
    TestPluginModel
    TestSettingsRegistry
    TestTransfer
)

# Set up targets for each of the tests
foreach(_test ${TESTS})
    add_executable(${_test} ${_test}.cpp)
    set_target_properties(${_test} PROPERTIES
        CXX_STANDARD             11
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    )
    target_include_directories(${_test} PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")
    target_link_libraries(${_test} nitroshare mock Qt5::Test)
    add_test(NAME ${_test}
        COMMAND ${_test}
    )
endforeach()

# Ensure that the libnitroshare library is copied here for the tests
if(WIN32)
    add_custom_target(copy_libnitroshare ALL
        COMMAND "${CMAKE_COMMAND}" -E
            copy_if_different \"$<TARGET_FILE:nitroshare>\" \"${CMAKE_CURRENT_BINARY_DIR}\"
        DEPENDS nitroshare
    )
endif()
