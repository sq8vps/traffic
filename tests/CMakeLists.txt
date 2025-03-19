cmake_minimum_required(VERSION 3.14)
project(helperTest)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/4902ea2d7c6faed89b6facee00baa34bb108fc0d.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../sim)

add_executable(
  helperTest
  helperTest.cpp
)
target_link_libraries(
  helperTest
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(helperTest)
