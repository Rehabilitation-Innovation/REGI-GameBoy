# Search for all source files in the current directory
# and save the names to the DIR_font_SRCS variable
aux_source_directory(. DIR_font_SRCS)

# Generate link library
add_library(font ${DIR_font_SRCS})
target_link_libraries(font PUBLIC)
