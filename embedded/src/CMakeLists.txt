cmake_minimum_required(VERSION 3.12)

# -------------------------------------------------
# Pico SDK discovery
# -------------------------------------------------
# Priority:
# 1) External -DPICO_SDK_PATH or env var
# 2) repo_root/pico-sdk
if(NOT DEFINED PICO_SDK_PATH)
    if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../../pico-sdk/pico_sdk_import.cmake")
        set(PICO_SDK_PATH "${CMAKE_CURRENT_LIST_DIR}/../../pico-sdk")
    else()
        message(FATAL_ERROR
            "PICO_SDK_PATH not set and pico-sdk not found.\n"
            "Expected at: repo_root/pico-sdk\n"
            "Or pass -DPICO_SDK_PATH=/path/to/pico-sdk"
        )
    endif()
endif()

include(${PICO_SDK_PATH}/pico_sdk_import.cmake)

# -------------------------------------------------
# Project
# -------------------------------------------------
project(humidity-sensor C CXX ASM)

set(PICO_BOARD pico_w)
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_C_STANDARD 11)


# For clangd / IDEs
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

pico_sdk_init()

# -------------------------------------------------
# Sources
# -------------------------------------------------
add_executable(humidity-sensor
    main.c
    hardware/buttons.c
    hardware/dht20_sensor.c
    hardware/lcd_i2c.c
    hardware/led_array.c
    hardware/photores.c
    hardware/photores.c
    core1/core1.c
    ui/lcd_screens.c
    ui/led_ui.c

)
target_include_directories(humidity-sensor PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

pico_enable_stdio_usb(humidity-sensor 1)


target_link_libraries(humidity-sensor
    pico_stdlib
    pico_multicore
    hardware_adc
    hardware_i2c
)

target_compile_options(humidity-sensor PRIVATE
    -Wall
    -Wno-format
    -Wno-unused-function
    -Wno-maybe-uninitialized
)

pico_add_extra_outputs(humidity-sensor)
