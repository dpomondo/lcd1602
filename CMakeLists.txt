cmake_minimum_required( VERSION 3.16)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PICO_SDK_PATH "/home/dpoyesac/Projects/pico/pico-sdk")
include(pico_sdk_import.cmake)

project(lcd1602 C CXX ASM)

pico_sdk_init()

add_library(lcd1602
    lcd1602_i2c.h
    lcd1602_i2c.c
    )

target_link_libraries(lcd1602
        pico_stdlib
        hardware_i2c
        )

target_include_directories(lcd1602 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
