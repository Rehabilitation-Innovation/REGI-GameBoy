// --------------------------------------------------------------------------
// ST7735-library (hw-specific defines and interfaces)
//
// If you want to port the library to a different platform, change this
// include (pins and ports, includes, function-map at the end of the file).
//
// Author: Bernhard Bablok
//
// https://github.com/bablokb/pico-st7735
// --------------------------------------------------------------------------

#ifndef _HW_H
#define _HW_H

#include "hardware/spi.h"
#include "pico/stdlib.h"

// ----------------------------------------------------------------
// pin and ports (usually defined in the makefile)

#ifndef SPI_TFT_PORT
#error "SPI port not defined; define SPI_TFT_PORT before including hw.h"
#endif

// chip-select output pin
#ifndef SPI_TFT_CS
#error "SPI CS not defined; define SPI_TFT_CS before including hw.h"
#endif

// TFT_DC output pin
#ifndef SPI_TFT_DC
#error "SPI DC not defined; define SPI_TFT_DC before including hw.h"
#endif

// TFT_RST output pin
#ifndef SPI_TFT_RST
#error "SPI port not defined; define SPI_TFT_PORT before including hw.h"
#endif
// ----------------------------------------------------------------

// ----------------------------------------------------------------
// helper macros

// ----------------------------------------------------------------
// declerations
void tft_spi_init();

// ----------------------------------------------------------------
// necessary includes

#include "hardware/spi.h"

// ----------------------------------------------------------------
// function-map
#ifdef __delay_ms
#undef __delay_ms
#endif
#define __delay_ms(x) sleep_ms(x)

#define spiwrite(data) spi_write_blocking(SPI_TFT_PORT, &data, 1)

#define tft_cs_low()                 \
  asm volatile("nop \n nop \n nop"); \
  gpio_put(SPI_TFT_CS, 0);           \
  asm volatile("nop \n nop \n nop")
#define tft_cs_high()                \
  asm volatile("nop \n nop \n nop"); \
  gpio_put(SPI_TFT_CS, 1);           \
  asm volatile("nop \n nop \n nop")

#define tft_dc_low()                 \
  asm volatile("nop \n nop \n nop"); \
  gpio_put(SPI_TFT_DC, 0);           \
  asm volatile("nop \n nop \n nop")
#define tft_dc_high()                \
  asm volatile("nop \n nop \n nop"); \
  gpio_put(SPI_TFT_DC, 1);           \
  asm volatile("nop \n nop \n nop")

#define tft_rst_low()                \
  asm volatile("nop \n nop \n nop"); \
  gpio_put(SPI_TFT_RST, 0);          \
  asm volatile("nop \n nop \n nop")
#define tft_rst_high()               \
  asm volatile("nop \n nop \n nop"); \
  gpio_put(SPI_TFT_RST, 1);          \
  asm volatile("nop \n nop \n nop")
// ----------------------------------------------------------------

#endif
