cmake_minimum_required(VERSION 3.13)

project (mrtp_cli)

file(GLOB SRC_FILES
    src/*.cpp
    src/actors/*.cpp
    /usr/include/easylogging++.cc
    ${PROJECT_SOURCE_DIR}/thirdparty/toojpeg/toojpeg.cpp
    ${PROJECT_SOURCE_DIR}/thirdparty/lodepng/lodepng.cpp
)

add_executable(mrtp_cli ${SRC_FILES})

target_include_directories(mrtp_cli
    PRIVATE 
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/thirdparty/cpptoml/include
        ${PROJECT_SOURCE_DIR}/thirdparty/CLI11/include
        ${PROJECT_SOURCE_DIR}/thirdparty/toojpeg
        ${PROJECT_SOURCE_DIR}/thirdparty/lodepng
        /usr/include/eigen3
)

target_link_libraries(mrtp_cli PRIVATE m 3ds)

set(GCC_COVERAGE_COMPILE_FLAGS "-W -Wall -pedantic -O2")

add_definitions(${GCC_COVERAGE_COMPILE_FLAGS})
