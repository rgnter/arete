cmake_minimum_required(
        VERSION 3.18)

project(arete
    LANGUAGES CXX
    DESCRIPTION "Full realization of potential or inherent function of modern real-time game engines")

add_subdirectory(3rd-party)

add_library(project_properties INTERFACE)
target_compile_features(project_properties
        INTERFACE cxx_std_20)

add_subdirectory(engine)
add_subdirectory(editor)
