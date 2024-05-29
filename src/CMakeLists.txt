add_library(loop loop.hpp loop.cpp)
add_library(seed seed.hpp seed.cpp)

add_executable(fuzz_main fuzz_main.cpp)
target_link_libraries(fuzz_main loop seed)

add_executable(shd_mem_example shd_mem_example.cpp)
