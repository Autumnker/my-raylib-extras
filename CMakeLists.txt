cmake_minimum_required(VERSION 3.16)
project(my-raylib-extras LANGUAGES C CXX)

set(CMAKE_C_STANDARD  11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED  ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/utf-8)   # avoid C4819 with Unicode source files
endif()

# ------------------------------------------------------------------
# Raylib — adjust RAYLIB_DIR or let it default
# ------------------------------------------------------------------
if(NOT DEFINED RAYLIB_DIR)
    set(RAYLIB_DIR "${CMAKE_SOURCE_DIR}/../raylib-5.5_win64_mingw-w64")
endif()

# The default raylib package is MinGW-w64; MSVC cannot link against it.
get_filename_component(RAYLIB_DIR_ABS "${RAYLIB_DIR}" ABSOLUTE)
if(MSVC AND EXISTS "${RAYLIB_DIR_ABS}/lib/libraylib.a")
    message(FATAL_ERROR
        "The raylib at\n  ${RAYLIB_DIR_ABS}\n"
        "was built for MinGW-w64 and cannot be linked by MSVC.\n"
        "Re-run CMake with the MinGW generator:\n"
        "  cmake -B build -G \"MinGW Makefiles\" -D RAYLIB_DIR=...\n"
        "Or download the MSVC raylib package:\n"
        "  https://github.com/raysan5/raylib/releases")
endif()

set(RAYLIB_INCLUDE "${RAYLIB_DIR}/include")
set(RAYLIB_LIB   "${RAYLIB_DIR}/lib")

# raylib on Windows / MinGW additionally needs: opengl32 gdi32 winmm
if(WIN32)
    set(RAYLIB_EXTRA_LIBS opengl32 gdi32 winmm)
endif()

# ------------------------------------------------------------------
# Helper — add a C-language example target
#
# Usage: add_c_example(rmouse)
#
# Expects:
#   src/rmouse/rmouse.h             — the C library
#   examples/rmouse/example_rmouse.c       — the C example source
# ------------------------------------------------------------------
function(add_c_example EXAMPLE_NAME)
    set(EXE_NAME "${EXAMPLE_NAME}_c_example")
    add_executable(${EXE_NAME} examples/${EXAMPLE_NAME}/example_${EXAMPLE_NAME}.c)

    set_target_properties(${EXE_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        C_STANDARD 11
    )

    target_include_directories(${EXE_NAME} PRIVATE
        src/${EXAMPLE_NAME}        # for "${EXAMPLE_NAME}.h"
        ${RAYLIB_INCLUDE}         # for "raylib.h"
    )

    target_link_directories(${EXE_NAME} PRIVATE ${RAYLIB_LIB})
    target_link_libraries(${EXE_NAME} PRIVATE
        raylib
        ${RAYLIB_EXTRA_LIBS}
    )
endfunction()

# ------------------------------------------------------------------
# Helper — add a C++ example target
#
# Usage: add_cpp_example(rmouse)
#
# Expects:
#   src/rmouse/rmouse.hpp              — the C++ library
#   examples/rmouse/example_rmouse.cpp        — the C++ example source
# ------------------------------------------------------------------
function(add_cpp_example EXAMPLE_NAME)
    set(EXE_NAME "${EXAMPLE_NAME}_cpp_example")
    add_executable(${EXE_NAME} examples/${EXAMPLE_NAME}/example_${EXAMPLE_NAME}.cpp)

    set_target_properties(${EXE_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        CXX_STANDARD 17
    )

    target_include_directories(${EXE_NAME} PRIVATE
        src/${EXAMPLE_NAME}          # for "rmouse.hpp"
        ${RAYLIB_INCLUDE}           # for "raylib.h"
    )

    target_link_directories(${EXE_NAME} PRIVATE ${RAYLIB_LIB})
    target_link_libraries(${EXE_NAME} PRIVATE
        raylib
        ${RAYLIB_EXTRA_LIBS}
    )
endfunction()

# ==================================================================
# C examples   — add one line per C library below
# ==================================================================
add_c_example(rmouse)

# ==================================================================
# C++ examples — add one line per C++ library below
# ==================================================================
add_cpp_example(rmouse)
# add_cpp_example(rfuture)       # ← uncomment when you create src/rfuture/
