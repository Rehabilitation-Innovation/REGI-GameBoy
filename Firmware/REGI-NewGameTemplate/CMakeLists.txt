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
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
# set(PICO_BOARD waveshare_rp2350_plus_16mb CACHE STRING "Board type")
# set(PICO_BOARD_HEADER_DIRS ${CMAKE_SOURCE_DIR})
set(PICO_BOARD_HEADER_DIRS ../REGI-TinyEngine/boards)
set(PICO_BOARD regi_rev1 CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(REGI-NewGameTemplate C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

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
    "inc/*.h"
    "inc/**.h"
)

# https://stackoverflow.com/questions/7980784/cmake-add-sub-directory-which-is-not-sub-directory-on-real-directory
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../REGI-TinyEngine ${CMAKE_CURRENT_BINARY_DIR}/lib/REGI-TinyEngine)

add_subdirectory(lib/pico-distance-sensor)

# Add executable. Default name is the project name, version 0.1
add_executable(${PROJECT_NAME} ${SOURCES} ${INCLUDES})

pico_set_program_name(${PROJECT_NAME} ${PROJECT_NAME})
pico_set_program_version(${PROJECT_NAME} "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(${PROJECT_NAME} 1)
pico_enable_stdio_usb(${PROJECT_NAME} 0)

# Add the standard library to the build
target_link_libraries(${PROJECT_NAME} PUBLIC
    pico_stdlib

    REGI-TinyEngine
)

# Add the standard include files to the build
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    "inc/"
    ${CMAKE_CURRENT_BINARY_DIR}/lib/REGI-TinyEngine/inc
)

pico_add_extra_outputs(${PROJECT_NAME})

pico_set_linker_script(${CMAKE_PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/../REGI-TinyEngine/memmap_default_rp2350.ld) # only making for r2350