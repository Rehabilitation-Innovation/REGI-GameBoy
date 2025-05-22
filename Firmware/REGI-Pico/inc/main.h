#ifndef __MAIN_H__
#define __MAIN_H__

#include "hardware/spi.h"

/* GPIO Numbers */

// also defined in Cmake as compile definitions
#define LCD_SPI_PORT spi1
#define LCD_SPI_MISO 12
#define LCD_SPI_MOSI 11
#define LCD_SPI_CLK 10
#define LCD_DC 8
#define LCD_RST 9
#define LCD_CS 13
#define LCD_BKLT 6
// #define LCD_SPI_SPEED 1000000  // 2 MHz

#endif