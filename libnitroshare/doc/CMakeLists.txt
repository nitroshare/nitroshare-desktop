# If enabled, download the tagfiles for Qt
if(DOWNLOAD_TAGFILES)
    foreach(_tagfile qtcore.tags qtnetwork.tags)
        set(_tagfile_path "${CMAKE_CURRENT_BINARY_DIR}/${_tagfile}")

        # Download the tagfile if it does not exist
        if(NOT EXISTS "${_tagfile_path}")
            file(DOWNLOAD "http://qt-project.org/doc/${_tagfile}"
                "${_tagfile_path}"
                SHOW_PROGRESS
            )
        endif()

        # Add the tagfile to the list
        list(APPEND TAGFILES ${_tagfile}=http://qt-project.org/doc/)
    endforeach()
endif()

# Convert the list into a string with semicolons replaced by spaces
string(REPLACE ";" " " TAGFILES "${TAGFILES}")

configure_file(Doxyfile.in "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile")

# Create a doc target for generating the documentation
add_custom_target(doc ALL
    "${DOXYGEN_EXECUTABLE}" Doxyfile
)

# TODO: install documentation to an appropriate location
