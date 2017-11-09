configure_file(Doxyfile.in "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile")

# Create a doc target for generating the documentation
add_custom_target(doc ALL
    "${DOXYGEN_EXECUTABLE}" "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
)

# TODO: install documentation to an appropriate location
