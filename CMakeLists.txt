cmake_minimum_required(VERSION 3.13)

project(mrtp)

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

option(USE_OPENMP "Enable OpenMP support" ON)
if(USE_OPENMP)
    find_package(OpenMP)
    if (OpenMP_CXX_FOUND)
        target_link_libraries(mrtp_cli PUBLIC OpenMP::OpenMP_CXX)
    endif()
endif()

option(USE_EASYLOGGING "Enable Easylogging++ support" OFF)
if(USE_EASYLOGGING)
    add_library(easyloggingpp STATIC thirdparty/easyloggingpp/src/easylogging++.cc)
    target_include_directories(easyloggingpp PUBLIC thirdparty/easyloggingpp/src)

    target_compile_definitions(mrtp_cli PRIVATE USE_EASYLOGGING)
    target_link_libraries(mrtp_cli PUBLIC easyloggingpp)
endif()

option(USE_LIB3DS "Enable support for models in 3D Studio format" OFF)
if(USE_LIB3DS)
    target_compile_definitions(mrtp_cli PRIVATE USE_LIB3DS)
    target_link_libraries(mrtp_cli PUBLIC 3ds)
endif()

option(USE_OPENBABEL "Use Open Babel for reading MOL2 format" OFF)
if(USE_OPENBABEL)
    target_compile_definitions(mrtp_cli PRIVATE USE_OPENBABEL)
    target_include_directories(mrtp_cli PUBLIC /usr/include/openbabel-2.0)
    target_link_libraries(mrtp_cli PUBLIC openbabel)
endif()

target_link_libraries(mrtp_cli PUBLIC m lodepng toojpeg)


