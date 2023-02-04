cmake_minimum_required(VERSION 3.13)

project(mrtp LANGUAGES CXX)

set(GCC_COVERAGE_COMPILE_FLAGS "-W -Wall -pedantic -O2")
add_definitions(${GCC_COVERAGE_COMPILE_FLAGS})

add_library(lodepng STATIC thirdparty/lodepng/lodepng.cpp)
target_include_directories(lodepng PUBLIC thirdparty/lodepng)

add_library(toojpeg STATIC thirdparty/toojpeg/toojpeg.cpp)
target_include_directories(toojpeg PUBLIC thirdparty/toojpeg)

add_executable(mrtp_cli "")
add_subdirectory("src")
add_subdirectory("src/actors")
target_include_directories(mrtp_cli PUBLIC src thirdparty/eigen thirdparty/cpptoml/include thirdparty/CLI11/include)

target_link_libraries(mrtp_cli PUBLIC m lodepng toojpeg)
