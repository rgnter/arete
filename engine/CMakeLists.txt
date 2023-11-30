add_library(engine SHARED
       src/main.cpp
       src/core.cpp
       src/vulkan.cpp)

find_package(Vulkan REQUIRED)

target_link_libraries(engine
        PRIVATE project_properties
        PUBLIC glfw glm Vulkan::Vulkan)
target_include_directories(engine
        PUBLIC include/)