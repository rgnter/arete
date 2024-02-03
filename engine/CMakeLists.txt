include(cmake/resources.cmake)
find_package(Vulkan REQUIRED)

add_library(engine STATIC
        src/core.cpp
        src/vulkan/display.cpp
        src/vulkan/renderer.cpp
        src/vulkan/engine.cpp
        src/engine.cpp
        src/input/input.cpp
        src/input/glfwInput.cpp
        src/tickClock.cpp
        include/arete/composer.hpp)

set_target_properties(engine PROPERTIES
        POSITION_INDEPENDENT_CODE ON)

target_compile_shaders(engine
        resources/shaders)
target_link_libraries(engine
        PUBLIC project_properties
        PUBLIC glfw glm Vulkan::Vulkan)
target_include_directories(engine
        PUBLIC include/)

add_executable(game
        src/main.cpp)
target_link_libraries(game
        PRIVATE engine)