# Search for all source files in the current directory
# and save the names to the DIR_Config_SRCS variable
aux_source_directory(. DIR_CONFIG_SRCS)

# Generate link library
add_library(config ${DIR_CONFIG_SRCS})
target_link_libraries(config PUBLIC pico_stdlib hardware_spi)
