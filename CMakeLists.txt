cmake_minimum_required(VERSION 3.16)
project(my-raylib-extras LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
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
# Helper — add a single-library example target
#
# Usage: add_raylib_example(rmouse)
#
# Expects:
#   src/<name>/<name>.h       — the header-only library
#   examples/<name>/main.cpp     — the example source
# ------------------------------------------------------------------
function(add_raylib_example EXAMPLE_NAME)
    set(EXE_NAME "${EXAMPLE_NAME}_example")
    add_executable(${EXE_NAME} examples/${EXAMPLE_NAME}/main.cpp)

    set_target_properties(${EXE_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )

    target_include_directories(${EXE_NAME} PRIVATE
        src/${EXAMPLE_NAME}       # for "${EXAMPLE_NAME}.h"
        ${RAYLIB_INCLUDE}        # for "raylib.h"
    )

    target_link_directories(${EXE_NAME} PRIVATE ${RAYLIB_LIB})
    target_link_libraries(${EXE_NAME} PRIVATE
        raylib
        ${RAYLIB_EXTRA_LIBS}
    )
endfunction()

# ==================================================================
# Examples — add one line per library below
# ==================================================================
add_raylib_example(rmouse)
# add_raylib_example(rfuture)     # ← uncomment when you create src/rfuture/
