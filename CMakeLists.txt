cmake_minimum_required(VERSION 3.25)

set(NAME aoc2022)
project(${NAME})

set(CMAKE_CXX_STANDARD 20)

include_directories("${PROJECT_SOURCE_DIR}/src")

file(GLOB lib_src "${PROJECT_SOURCE_DIR}/src/*.cpp")

if (lib_src)
    add_library("${NAME}"
        ${lib_src}
    )
endif()

file(GLOB bin_files "${PROJECT_SOURCE_DIR}/src/bin/*.cpp")
foreach(cpp_file ${bin_files})
    cmake_path(GET cpp_file STEM bin_stem)
    add_executable("${bin_stem}" "${cpp_file}")
    if (lib_src)
        target_link_libraries("${bin_stem}" "${NAME}")
    endif()
endforeach(cpp_file ${bin_files})
