cmake_minimum_required(VERSION 3.5)
project(easyLog VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

add_executable(thread_test thread_test.cxx)
target_compile_features(thread_test PRIVATE cxx_std_20)
add_test(NAME thread COMMAND thread_test)