cmake_minimum_required(VERSION 3.12)

# import SDK
set(PICO_SDK_PATH ${CMAKE_CURRENT_LIST_DIR}/../pico-sdk)
include(${PICO_SDK_PATH}/pico_sdk_import.cmake)
#include(pico_sdk_import.cmake)

# set the name of the project
project(humidity-sensor)

# building for original pico board
set(PICO_BOARD "pico")

# define CMake build type
set(CMAKE_BUILD_TYPE Debug)

# define C standard
set(CMAKE_C_STANDARD 11)

# define compiler
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)

# use this to prevent clang-tidy from failing to find pico libs
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

# initialize SDK
pico_sdk_init()

# define files & libraries
add_executable(humidity-sensor
    main.c
    hardware/lcd.c
    hardware/led_array.c
    ui/lcd_ui.c
    ../mock/mock_ui.c
)

target_include_directories(humidity-sensor PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    hardware
    ui
    ../mock
)

pico_enable_stdio_usb(humidity-sensor 1)

target_link_libraries(humidity-sensor
    pico_stdlib
    hardware_i2c
    hardware_gpio
)

add_compile_options(-Wall
        -Wno-format
        -Wno-unused-function
        -Wno-maybe-uninitialized
        )

# generate .uf2 file to put on pico
pico_add_extra_outputs(humidity-sensor)
