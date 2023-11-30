add_executable(editor
    src/main.cpp)
target_link_libraries(editor
    PRIVATE project_properties engine)