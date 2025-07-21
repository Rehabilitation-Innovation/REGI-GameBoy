# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
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
set(PICO_BOARD pico2 CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(REGI-CPP-Debugable C CXX ASM)

set(PICO_CXX_ENABLE_EXCEPTIONS 0)

set(PICO_CXX_ENABLE_RTTI 0)

file(GLOB SOURCES
    "src/**.c"
    "src/**.cpp"
    "src/scenes/**.cpp"
)

file(GLOB INCLUDES
    "src/**.h"
    "src/**.hpp"
    "src/scenes/**.h"
)

string(APPEND CMAKE_EXE_LINKER_FLAGS "-Wl,--print-memory-usage")

add_compile_options(-Wall -Wextra)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1
add_executable(REGI-CPP-Debugable ${SOURCES} ${INCLUDES}
)

pico_set_program_name(REGI-CPP-Debugable "REGI-CPP-Debugable")
pico_set_program_version(REGI-CPP-Debugable "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(REGI-CPP-Debugable 1)
pico_enable_stdio_usb(REGI-CPP-Debugable 0)

pico_set_float_implementation(REGI-CPP-Debugable pico)
pico_set_double_implementation(REGI-CPP-Debugable pico)

target_include_directories(REGI-CPP-Debugable PRIVATE
    "inc/"
    "src/scenes/"
)

# Add the standard library to the build
target_link_libraries(REGI-CPP-Debugable
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

target_compile_definitions(REGI-CPP-Debugable PRIVATE
    PICO_DEFAULT_UART=0
    PICO_DEFAULT_UART_TX_PIN=28
    PICO_DEFAULT_UART_RX_PIN=29
)

# Add the standard include files to the build
target_include_directories(REGI-CPP-Debugable PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

pico_add_extra_outputs(REGI-CPP-Debugable)
