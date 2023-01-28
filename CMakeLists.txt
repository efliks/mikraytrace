cmake_minimum_required(VERSION 3.13)

project (mrtp_cli)

file(GLOB SRC_FILES
    src/*.cpp
    src/actors/*.cpp
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
        ${PROJECT_SOURCE_DIR}/thirdparty/eigen
)

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

target_link_libraries(mrtp_cli PRIVATE m)

set(GCC_COVERAGE_COMPILE_FLAGS "-W -Wall -pedantic -O2 -DUSE_OPENMP")
add_definitions(${GCC_COVERAGE_COMPILE_FLAGS})
