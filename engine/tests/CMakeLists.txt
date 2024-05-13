include(CTest)

enable_testing()

add_executable(engine_test)
target_sources(engine_test PRIVATE engine.cpp)
target_link_libraries(engine_test PRIVATE engine)

add_test(NAME engine_test COMMAND engine_test)
