add_library(engine SHARED
        src/core.cpp
        src/vulkan.cpp
        src/engine.cpp)

find_package(Vulkan REQUIRED)

target_link_libraries(engine
        PUBLIC project_properties
        PUBLIC glfw glm Vulkan::Vulkan)
target_include_directories(engine
        PUBLIC include/)

add_executable(game
        src/main.cpp)
target_link_libraries(game
        PRIVATE engine)