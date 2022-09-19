cmake_minimum_required(VERSION 3.19)
project(vk_graphics_stuff LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(
    $<$<CXX_COMPILER_ID:Clang>:-fcolor-diagnostics>
    $<$<CXX_COMPILER_ID:Clang>:-fansi-escape-codes>
)

add_library(default_interface INTERFACE)
target_compile_options(default_interface INTERFACE
    $<$<CXX_COMPILER_ID:Clang>:-Wall>
    $<$<CXX_COMPILER_ID:Clang>:-Wextra>
    $<$<CXX_COMPILER_ID:Clang>:-pedantic>
)

# --- OpenGL ---

find_package(OpenGL REQUIRED)

# --- Subdirectories ---

add_subdirectory(bag)
add_subdirectory(third_party)

# --- Executable ---

add_executable(main
    src/main.cpp
    src/common.cpp
    src/debug.cpp
    src/gameboy.cpp
    src/memory.cpp
    src/instruction.cpp
    src/cpu.cpp
)

target_include_directories(main
    PRIVATE src
    PRIVATE src/utils
    PRIVATE .
)

target_link_libraries(main
    default_interface
    bag
)

target_compile_definitions(main PRIVATE
    $<$<BOOL:${WIN32}>:NOMINMAX>
    $<$<BOOL:${WIN32}>:NOCOMM>
    $<$<BOOL:${WIN32}>:WIN32_LEAN_AND_MEAN>
    $<$<BOOL:${WIN32}>:VC_EXTRALEAN>
)

set_target_properties(main PROPERTIES
    CXX_STANDARD 20
    CXX_EXTENSIONS OFF
)
