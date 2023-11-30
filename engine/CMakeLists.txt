add_library(engine SHARED
       src/main.cpp)

find_package(Vulkan REQUIRED)

target_link_libraries(engine
        PRIVATE project_properties
        PUBLIC glfw glm Vulkan::Vulkan)