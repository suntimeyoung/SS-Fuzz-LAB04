add_library(loop loop.hpp loop.cpp)
add_library(seed seed.hpp seed.cpp)
add_library(bitmap bitmap.hpp bitmap.cpp)

add_executable(fuzz_main fuzz_main.cpp)
target_link_libraries(fuzz_main loop seed bitmap)

