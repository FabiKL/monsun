cmake_minimum_required(VERSION 3.16)
project(monsun_host_tests C)

set(CMAKE_C_STANDARD 11)
enable_testing()

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../libs/monsun_link monsun_link)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../libs/monsun_engine monsun_engine)

add_executable(monsun_tests test_main.c)
target_link_libraries(monsun_tests PRIVATE monsun_link monsun_engine)
add_test(NAME monsun_tests COMMAND monsun_tests)
