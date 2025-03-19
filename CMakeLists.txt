cmake_minimum_required(VERSION 3.10.0)
project(traffic VERSION 0.1.0 LANGUAGES C)

add_subdirectory(sim)

add_executable(traffic main.c json.c)

target_link_libraries(traffic PRIVATE SimLib)

enable_testing()
add_subdirectory(tests)
add_subdirectory(examples)