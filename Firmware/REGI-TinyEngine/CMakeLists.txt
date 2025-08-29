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

set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)

if(EXISTS ${picoVscode})
    include(${picoVscode})
endif()

# ====================================================================================
# set(PICO_BOARD waveshare_rp2350_plus_16mb CACHE STRING "Board type")
set(PICO_BOARD_HEADER_DIRS boards/)
set(PICO_BOARD regi_rev1 CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(REGI-TinyEngine C CXX ASM)

set(PICO_CXX_ENABLE_EXCEPTIONS 1)

set(PICO_CXX_ENABLE_RTTI 1)

# set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR} CACHE PATH "Install prefix set to project root" FORCE)
# add_subdirectory(lib/pico-vfs-main) # modified vfs with asserts updated.

# add_subdirectory(lib/pico-distance-sensor-main/pico-distance-sensor-main)

# add_subdirectory(lib/pi-pico-tone)
# add_subdirectory(lib/pico-pwm-audio)

# This function sets the custom linker script for SD card applications
# The memmap_sdcard_app.ld script configures the application to start at
# FLASH origin = 0x10000000 + 256k, which aligns with the SD_BOOT_FLASH_OFFSET
# defined in main.c (256 * 1024). This ensures the bootloader (first 256KB)
# is preserved when flashing the application.
function(enable_sdcard_app target)
    # #pico_set_linker_script(${target} ${CMAKE_SOURCE_DIR}/memmap_sdcard_app.ld)
    # if(${PICO_PLATFORM} STREQUAL "rp2040")
    # pico_set_linker_script(${CMAKE_PROJECT_NAME} ${CMAKE_SOURCE_DIR}/memmap_default_rp2040.ld)
    # elseif(${PICO_PLATFORM} MATCHES "rp2350")
    pico_set_linker_script(${CMAKE_PROJECT_NAME} ${CMAKE_SOURCE_DIR}/memmap_default_rp2350.ld) # only making for r2350

    # endif()
endfunction()

file(GLOB SOURCES
    "src/**.c"
    "src/**.cpp"
    "src/scenes/**.cpp"
)

file(GLOB INCLUDES
    "src/**.h"
    "src/**.hpp"
    "src/scenes/**.h"
    "inc/**.pio"
)

string(APPEND CMAKE_EXE_LINKER_FLAGS "-Wl,--print-memory-usage")

add_compile_options(-Wall -Wextra)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1
# add_executable(REGI-TinyEngine ${SOURCES} ${INCLUDES})

# )
add_library(REGI-TinyEngine ${SOURCES} ${INCLUDES})
pico_set_program_name(REGI-TinyEngine "REGI-TinyEngine")
pico_set_program_version(REGI-TinyEngine "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(REGI-TinyEngine 1)
pico_enable_stdio_usb(REGI-TinyEngine 0)

# pico_set_float_implementation(REGI-TinyEngine pico)
# pico_set_double_implementation(REGI-TinyEngine pico)
target_include_directories(REGI-TinyEngine PUBLIC
    "inc/"
    "src/scenes/"
    "lib/pico-distance-sensor-main/pico-distance-sensor-main/include"
)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/src/generated)

# generate the header file into the source tree as it is included in the RP2040 datasheet
pico_generate_pio_header(REGI-TinyEngine ${CMAKE_CURRENT_LIST_DIR}/inc/ws2812.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/src/generated)

# Add the standard library to the build
target_link_libraries(REGI-TinyEngine PUBLIC
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
    hardware_adc
    hardware_exception

    pico_stdlib
    pico_rand
    pico_multicore
    pico_sync

    # blockdevice_sd
    # filesystem_fat
    # filesystem_vfs

    # pico_tone
    # distance-sensor
)

target_compile_definitions(REGI-TinyEngine PUBLIC
    PICO_DEFAULT_UART=0
    PICO_DEFAULT_UART_TX_PIN=28
    PICO_DEFAULT_UART_RX_PIN=29
)

# Add the standard include files to the build
target_include_directories(REGI-TinyEngine PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

# pico_add_extra_outputs(REGI-TinyEngine)
# enable_sdcard_app(REGI-TinyEngine)
