set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set (BUILD_OPTIONS
    $<$<COMPILE_LANGUAGE:CXX>:-Wall>
    $<$<COMPILE_LANGUAGE:CXX>:-Wcast-align>
    $<$<COMPILE_LANGUAGE:CXX>:-Wconversion>
    $<$<COMPILE_LANGUAGE:CXX>:-Werror>
    $<$<COMPILE_LANGUAGE:CXX>:-Wextra>
    $<$<COMPILE_LANGUAGE:CXX>:-Wfloat-equal>
    $<$<COMPILE_LANGUAGE:CXX>:-Wmissing-declarations>
    $<$<COMPILE_LANGUAGE:CXX>:-Wmissing-include-dirs>
    $<$<COMPILE_LANGUAGE:CXX>:-Wnon-virtual-dtor>
    $<$<COMPILE_LANGUAGE:CXX>:-Wold-style-cast>
    $<$<COMPILE_LANGUAGE:CXX>:-Woverloaded-virtual>
    $<$<COMPILE_LANGUAGE:CXX>:-Wredundant-decls>
    $<$<COMPILE_LANGUAGE:CXX>:-Wshadow>
    $<$<COMPILE_LANGUAGE:CXX>:-Wsign-conversion>
    $<$<COMPILE_LANGUAGE:CXX>:-Wswitch-default>
    $<$<COMPILE_LANGUAGE:CXX>:-Wswitch-enum>
    $<$<COMPILE_LANGUAGE:CXX>:-Wunused-result>
    $<$<COMPILE_LANGUAGE:CXX>:-pedantic>
    $<$<CONFIG:Debug>:-O0>
)

add_compile_options(
    ${BUILD_OPTIONS}
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_VERBOSE_MAKEFILE ON)