set(EXAMPLES
    browser
    provider
)

foreach(EXAMPLE ${EXAMPLES})
    set(EXAMPLE_DIR "${EXAMPLES_INSTALL_DIR}/${EXAMPLE}")
    add_subdirectory(${EXAMPLE})
    install(DIRECTORY ${EXAMPLE}
        DESTINATION "${EXAMPLES_INSTALL_DIR}"
        COMPONENT examples
    )
endforeach()
