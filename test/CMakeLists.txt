include_directories (${CHINA_FUZZ_LOOP_SOURCE_DIR}/src)

add_executable(seed_test seed_test.cpp)
target_link_libraries(seed_test seed)
add_test(NAME seed_test COMMAND seed_test)

