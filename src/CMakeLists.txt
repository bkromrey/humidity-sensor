cmake_minimum_required(VERSION 3.12)

set(PICO_SDK_PATH ${CMAKE_CURRENT_LIST_DIR}/../pico-sdk)
include(${PICO_SDK_PATH}/pico_sdk_import.cmake)

project(humidity-sensor)

set(PICO_BOARD "pico")
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_STANDARD 11)

pico_sdk_init()

add_executable(humidity-sensor
    main.c
    hardware/lcd.c
    hardware/led_array.c
    hardware/buttons.c      
    ui/lcd_ui.c
    core1/core1.c            
    data_flow/data_flow.h
  
)

target_include_directories(humidity-sensor PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    hardware
    ui
    core1
    data_flow

)

pico_enable_stdio_usb(humidity-sensor 1)
pico_enable_stdio_uart(humidity-sensor 0)

target_link_libraries(humidity-sensor
    pico_stdlib
    hardware_i2c
    hardware_gpio
    hardware_adc
    pico_multicore
    hardware_sync
)

add_compile_options(
    -Wall
    -Wno-format
    -Wno-unused-function
    -Wno-maybe-uninitialized
)

pico_add_extra_outputs(humidity-sensor)