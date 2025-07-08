# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()

set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)

if(EXISTS ${picoVscode})
    include(${picoVscode})
endif()

# ====================================================================================
set(PICO_BOARD waveshare_rp2350_plus_16mb CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(REGI-Pico-CPP C CXX ASM)

set(PICO_CXX_ENABLE_EXCEPTIONS 0)

set(PICO_CXX_ENABLE_RTTI 0)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

file(GLOB SOURCES
    "src/**.c"
    "src/**.cpp"
    "scenes/**.c"
)

file(GLOB INCLUDES
    "src/**.h"
    "src/**.hpp"
)

string(APPEND CMAKE_EXE_LINKER_FLAGS "-Wl,--print-memory-usage")

add_compile_options(-Wall -Wextra)

# Add executable. Default name is the project name, version 0.1
add_executable(REGI-Pico-CPP ${SOURCES} ${INCLUDES})

pico_set_float_implementation(REGI-Pico-CPP pico)
pico_set_double_implementation(REGI-Pico-CPP pico)

pico_set_program_name(REGI-Pico-CPP "REGI-Pico-CPP")
pico_set_program_version(REGI-Pico-CPP "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(REGI-Pico-CPP 1) # goes through the DAP Link
pico_enable_stdio_usb(REGI-Pico-CPP 0) # goes through the usb port, will use usb in future for keyboard.

target_include_directories(REGI-Pico-CPP PRIVATE
    "inc/"
)

# Add the standard library to the build
target_link_libraries(REGI-Pico-CPP
    hardware_spi
    hardware_i2c
    hardware_spi
    hardware_dma
    hardware_pio
    hardware_interp
    hardware_timer
    hardware_watchdog
    hardware_clocks
    hardware_pwm

    pico_stdlib
    pico_rand
    pico_multicore
    pico_sync
)

# Add the standard include files to the build
target_include_directories(REGI-Pico-CPP PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

pico_add_extra_outputs(REGI-Pico-CPP)
