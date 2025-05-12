# Search for all source files in the current directory
# and save the names to the DIR_FATFS_SRCS variable
aux_source_directory(. DIR_FATFS_SRCS)

include_directories(../config)
include_directories(../lcd)
include_directories(../font)
include_directories(../sdcard)

# Generate link library
add_library(fatfs ${DIR_FATFS_SRCS})
target_link_libraries(fatfs PUBLIC config hardware_spi lcd font sdcard)
