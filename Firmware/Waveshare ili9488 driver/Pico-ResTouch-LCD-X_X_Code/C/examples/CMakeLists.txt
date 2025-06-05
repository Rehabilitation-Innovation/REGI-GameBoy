# Search for all source files in the current directory
# and save the names to the DIR_examples_SRCS variable
aux_source_directory(. DIR_EXAMPLES_SRCS)

# Add header file directory
include_directories(../lib/config)
include_directories(../lib/lcd)
include_directories(../lib/font)
include_directories(../lib/fatfs)
include_directories(../lib/sdcard)

# Generate link library
add_library(examples ${DIR_EXAMPLES_SRCS})
target_link_libraries(examples PUBLIC config lcd font fatfs sdcard)
