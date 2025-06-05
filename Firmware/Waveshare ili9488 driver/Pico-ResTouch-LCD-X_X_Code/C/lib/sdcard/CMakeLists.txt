# Search for all source files in the current directory
# and save the names to the DIR_SDCARD_SRCS variable
aux_source_directory(. DIR_SDCARD_SRCS)

include_directories(../config)

# Generate link library
add_library(sdcard ${DIR_SDCARD_SRCS})
target_link_libraries(sdcard PUBLIC config)
