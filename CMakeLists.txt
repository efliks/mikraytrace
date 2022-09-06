cmake_minimum_required(VERSION 3.13)

project (mrtp_cli)

set(SOURCES
    src/actors.cpp
    src/babel.cpp
    src/camera.cpp
    src/light.cpp
    src/main.cpp
    src/mappers.cpp
    src/renderer.cpp
    src/texture.cpp
    src/world.cpp
    /usr/include/easylogging++.cc
)

add_executable(mrtp_cli ${SOURCES})

target_include_directories(mrtp_cli
    PRIVATE 
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/thirdparty/cpptoml/include
        /usr/include/eigen3
        /usr/include/png++
        /usr/include/openbabel-2.0
)

target_link_libraries(mrtp_cli PRIVATE png openbabel m)

set(GCC_COVERAGE_COMPILE_FLAGS "-W -Wall -pedantic -fPIC -O2")

add_definitions(${GCC_COVERAGE_COMPILE_FLAGS})
