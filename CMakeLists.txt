cmake_minimum_required(VERSION 3.16)
project(rmouse-example LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/utf-8)   # avoid C4819 with Unicode source files
endif()

# ------------------------------------------------------------------
# Raylib — adjust RAYLIB_DIR or let it default to the bundled version
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
# Example target
# ------------------------------------------------------------------
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR})
add_executable(example_demo examples/main.cpp)

target_include_directories(example_demo PRIVATE
    ${CMAKE_SOURCE_DIR}/src      # for "rmouse.h"
    ${RAYLIB_INCLUDE}            # for "raylib.h"
)

target_link_directories(example_demo PRIVATE ${RAYLIB_LIB})

target_link_libraries(example_demo PRIVATE
    raylib
    ${RAYLIB_EXTRA_LIBS}
)
