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
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(REGI-Pico C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# add_subdirectory(lib/config)
# add_subdirectory(lib/lcd)
# add_subdirectory(lib/font)

# # add_subdirectory(lib/fatfs)
# # add_subdirectory(lib/sdcard)
# include_directories(./lib/config)
# include_directories(./lib/lcd)
# include_directories(./lib/font)

# include_directories(./lib/fatfs)
# include_directories(./lib/sdcard)
file(GLOB SOURCES "src/*.c")

# library configuration (override if necessary)
set(SPI_TFT_PORT "spi1" CACHE STRING "TFT spi port-name")
set(SPI_TFT_CS "13" CACHE STRING "TFT CS  pin number")
set(SPI_TFT_DC "8" CACHE STRING "TFT DC  pin number")
set(SPI_TFT_RST "9" CACHE STRING "TFT RST pin number")
set(TFT_OPTIONS TFT_ENABLE_GREEN TFT_ENABLE_RESET TFT_ENABLE_TEXT TFT_ENABLE_SHAPES
    TFT_ENABLE_ROTATE TFT_ENABLE_SCROLL
    CACHE STRING "TFT options/functions")

# TFT options/functions. Complete list:
# - TFT_ENABLE_BLACK
# - TFT_ENABLE_GREEN
# - TFT_ENABLE_RED
# - TFT_ENABLE_GENERIC
# - TFT_ENABLE_TEXT
# - TFT_ENABLE_FONTS
# - TFT_ENABLE_ALL
# - TFT_ENABLE_RESET
# - TFT_ENABLE_SCROLL
# - TFT_ENABLE_SHAPES
# - TFT_ENABLE_ROTATE
# - TFT_ENABLE_BMP  (not implemented yet)
add_compile_definitions(SPI_TFT_PORT=${SPI_TFT_PORT})
add_compile_definitions(SPI_TFT_CS=${SPI_TFT_CS})
add_compile_definitions(SPI_TFT_DC=${SPI_TFT_DC})
add_compile_definitions(SPI_TFT_RST=${SPI_TFT_RST})

foreach(opt IN LISTS TFT_OPTIONS)
    message(">>> using: ${opt}")
    add_compile_definitions(${opt})
endforeach(opt)

# add preprocessor-constant DEBUG for Debug-builds
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_VERBOSE_MAKEFILE 1)
    add_compile_definitions(DEBUG)
else()
endif()

# Add executable. Default name is the project name, version 0.1
add_executable(REGI-Pico ${SOURCES})

target_include_directories(REGI-Pico PRIVATE
    "inc/"
)

pico_set_program_name(REGI-Pico "REGI-Pico")
pico_set_program_version(REGI-Pico "0.1")

# Generate PIO header
# pico_generate_pio_header(REGI-Pico ${CMAKE_CURRENT_LIST_DIR}/blink.pio)

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(REGI-Pico 1)

pico_enable_stdio_usb(REGI-Pico 1)

# Add the standard library to the build
target_link_libraries(REGI-Pico
    pico_stdlib)

add_subdirectory(lib/st7735)

# Add the standard include files to the build
target_include_directories(REGI-Pico PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
)

# Add any user requested libraries
target_link_libraries(REGI-Pico
    pico_stdlib
    hardware_spi
    st7735

    # config

    # font
    # lcd

    # fatfs
)

pico_add_extra_outputs(REGI-Pico)
