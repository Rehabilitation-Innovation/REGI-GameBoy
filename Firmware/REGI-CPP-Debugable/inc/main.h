#ifndef __MAIN_H__
#define __MAIN_H__

#include "hardware/spi.h"

/* GPIO Numbers */

// also defined in Cmake as compile definitions
#define LCD_SPI_PORT spi1
#define LCD_SPI_MISO 28
#define LCD_SPI_MOSI 11
#define LCD_SPI_CLK 10
#define LCD_DC 8
#define LCD_RST 9
#define LCD_CS 22
#define LCD_BKLT 6

// GPIOs for SPI interface (SD card)
#define SD_SPI0         0
#define SD_SCLK_PIN     34
#define SD_MOSI_PIN     35
#define SD_MISO_PIN     32
#define SD_CS_PIN       33
// #define SD_DET_PIN 22

// PicoCalc SD Firmware Loader
// SD_BOOT_FLASH_OFFSET is the offset in flash memory where the bootloader starts
// This is typically set to 256KB for the Raspberry Pi Pico
// This offset is used to ensure that the bootloader does not get overwritten
// when loading a new application from the SD card
#define SD_BOOT_FLASH_OFFSET         (256 * 1024)

// Maximum size of the application that can be loaded
// This ensures we don't overwrite the bootloader itself
#define MAX_APP_SIZE                 (PICO_FLASH_SIZE_BYTES - SD_BOOT_FLASH_OFFSET)


// static int condition =0;
// #define LCD_SPI_SPEED 1000000  // 2 MHz

#endif