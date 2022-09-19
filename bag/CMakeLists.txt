add_library(bag STATIC
    src/window.cpp
    src/image.cpp
    src/bag.cpp
)
target_include_directories(bag
    PUBLIC include/
    PRIVATE include/bag/
    PRIVATE ./
)
target_link_libraries(bag
    ${OPENGL_LIBRARIES}
    glad
    glfw
    imgui
)
set_property(TARGET bag PROPERTY CXX_STANDARD 20)
