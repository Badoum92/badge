# --- Third party interface ---

add_library(third_party_interface INTERFACE)
target_compile_options(third_party_interface INTERFACE
    $<$<CXX_COMPILER_ID:Clang>:-Wno-everything>
)

# --- GLFW ---

option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
option(GLFW_INSTALL OFF)
add_subdirectory(glfw)

# --- GLAD ---

add_library(glad STATIC glad/src/glad.c)
target_include_directories(glad PUBLIC glad/include)
target_link_libraries(glad PRIVATE third_party_interface)

# --- IMGUI ---

file(GLOB IMGUI_SOURCES
    imgui/*.cpp
    imgui/backends/imgui_impl_glfw.cpp
    imgui/backends/imgui_impl_opengl3.cpp
)
add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC
    ./
    imgui/
    imgui/backends
    glfw/include
    glad/include
)
target_link_libraries(imgui PUBLIC
    PUBLIC ${OPENGL_LIBRARIES}
    PUBLIC glfw
    PUBLIC glad
    PRIVATE third_party_interface
)
set_target_properties(imgui PROPERTIES LINKER_LANGUAGE CXX)
add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_GLAD)
