# Compiler warnings configuration for Modern C++
function(set_project_warnings project_name)
    target_compile_options(${project_name} PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Wcast-align
        -Wunused
        -Woverloaded-virtual
        -Wconversion
        -Wsign-conversion
    )
endfunction()
