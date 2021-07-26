find_package(GTest CONFIG REQUIRED)

add_executable(test_utils test_utils.cpp)

target_link_libraries(
  test_utils
  projects_generator_lib
  GTest::gtest
  GTest::gtest_main)

add_test(NAME test COMMAND test_utils)
