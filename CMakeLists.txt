cmake_minimum_required(VERSION 3.13)

project(mrtp CXX)

set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "OpenWatcom")
    set(GCC_COVERAGE_COMPILE_FLAGS "-W -Wall -pedantic -O2")
    add_definitions(${GCC_COVERAGE_COMPILE_FLAGS})
else()
    # -ot: optimize for time, Watcom's equivalent of GCC's -O2
    add_definitions(-ot)
endif()

add_library(lodepng STATIC thirdparty/lodepng/lodepng.cpp)
target_include_directories(lodepng PUBLIC thirdparty/lodepng)

add_library(toojpeg STATIC thirdparty/toojpeg/toojpeg.cpp)
target_include_directories(toojpeg PUBLIC thirdparty/toojpeg)

add_executable(mrtp_cli "")
add_subdirectory("src")
add_subdirectory("src/actors")
target_include_directories(mrtp_cli PUBLIC src)

option(USE_OPENMP "Enable OpenMP support" ON)
if(USE_OPENMP)
    find_package(OpenMP)
    if (OpenMP_CXX_FOUND)
        target_link_libraries(mrtp_cli PUBLIC OpenMP::OpenMP_CXX)
    endif()
endif()

option(USE_LIB3DS "Enable support for models in 3D Studio format" OFF)
if(USE_LIB3DS)
    target_compile_definitions(mrtp_cli PRIVATE USE_LIB3DS)
    target_link_libraries(mrtp_cli PUBLIC 3ds)
endif()

# Watcom's CRT already includes libm
target_link_libraries(mrtp_cli PUBLIC m lodepng toojpeg)
