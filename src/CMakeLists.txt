cmake_minimum_required(VERSION 3.12)

# import SDK
include(pico_sdk_import.cmake)

# set the name of the project
project(humidity-sensor)

# define CMake build type
set(CMAKE_BUILD_TYPE Debug)

# initialize SDK
pico_sdk_init()

# define files & libraries
add_executable(humidity-sensor
        main.c
        )

target_link_libraries(humidity-sensor pico_stdlib)

add_compile_options(-Wall
        -Wno-format
        -Wno-unused-function
        -Wno-maybe-uninitialized
        )

# generate .uf2 file to put on pico
pico_add_extra_outputs(humidity-sensor)
