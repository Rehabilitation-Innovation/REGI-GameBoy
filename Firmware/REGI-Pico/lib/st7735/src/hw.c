// --------------------------------------------------------------------------
// ST7735-library (hw-specific implementations)
//
// If you want to port the library to a different platform, change the
// implementation if necessary. Don't forget to change hw.h too.
//
// Author: Bernhard Bablok
//
// https://github.com/bablokb/pico-st7735
// --------------------------------------------------------------------------

#include "hw.h"

void tft_spi_init() {
    gpio_init(LCD_CS);
    gpio_set_dir(LCD_CS, GPIO_OUT);
    gpio_put(LCD_CS, 1);  // Chip select is active-low

    gpio_init(LCD_DC);
    gpio_set_dir(LCD_DC, GPIO_OUT);
    gpio_put(LCD_DC, 0);  // Chip select is active-low

    gpio_init(LCD_RST);
    gpio_set_dir(LCD_RST, GPIO_OUT);
    gpio_put(LCD_RST, 0);
}
