# The MIT License (MIT)
#
# Copyright (c) 2017 Nathan Osman
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

# Copy the runtime files for the src shared library to the dest target.
function(copy_lib_win src dest)
    add_custom_command(TARGET ${dest} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E
            copy_if_different \"$<TARGET_FILE:${src}>\" \"$<TARGET_FILE_DIR:${dest}>\"
        COMMENT "Copying ${src} to ${dest}..."
    )
endfunction()

# Copy the dynlib for the src shared library to the dest target and correct
# its RPATH.
function(copy_lib_mac src dest)
    add_custom_command(TARGET ${dest} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E
            make_directory "$<TARGET_FILE_DIR:${dest}>/../Frameworks"
        COMMAND "${CMAKE_COMMAND}" -E
            copy_if_different
                "$<TARGET_FILE:${src}>"
                "$<TARGET_FILE_DIR:${dest}>/../Frameworks/$<TARGET_SONAME_FILE_NAME:${src}>"
        COMMAND install_name_tool
            -change
            "$<TARGET_SONAME_FILE_NAME:${src}>"
            "@rpath/$<TARGET_SONAME_FILE_NAME:${src}>"
            "$<TARGET_FILE:${dest}>"
        COMMENT "Copying ${src} to ${dest} and correcting RPATH..."
    )
endfunction()
