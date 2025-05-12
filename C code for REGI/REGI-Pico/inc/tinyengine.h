#ifndef __TINYENGINE_H__
#define __TINYENGINE_H__

#include "hardware/spi.h"
#include <stdint.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#define telog(M, ...) \
  printf("[Tiny Engine Log] (%s:%d)" M "\r\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define teerr(M, ...) \
  printf("[Tiny Engine ERROR] (%s:%d)" M "\r\n", __FILE__, __LINE__, ##__VA_ARGS__)

typedef enum {
    TINYENGINE_OK,
    TINYENGINE_ERROR,
    TINYENGINE_RENDER_ERROR
} tinyengine_status_t;

typedef enum {
    TINYENGINE_RENDERER_VGA,
    TINYENGINE_RENDERER_LCD_ST7735R,  // 1.8 in adafruit green tab with sdcard
    TINYENGINE_RENDERER_LCD_ILI9488,  // 3.5 in waveshare dev kit with sdcard
} tinyengine_renderer_t;

typedef tinyengine_status_t(*tinyengine_init_clbk_t)();

typedef struct {
    tinyengine_renderer_t current_output_renderer;
} tinyengine_renderer_handle_t;

// The reason this is a uint8_t* in the paramenter is that its defined before the engine handle
// but it needs access to it. So to give it access i give it a uint8_t* instead of struct*
typedef tinyengine_status_t(*tinyengine_clbk_t)(float delta);

typedef struct {
    spi_inst_t* lcd_spi_instance;
    uint8_t lcd_spi_clk;
    uint8_t lcd_spi_miso;
    uint8_t lcd_spi_mosi;
    uint8_t lcd_spi_dc;
    uint8_t lcd_spi_rst;
    uint8_t lcd_spi_cs;
    uint8_t lcd_spi_bklt;
    uint32_t lcd_spi_speed;

    tinyengine_renderer_handle_t* render_engine_handle;

    tinyengine_renderer_t render_engine_to_use;

    tinyengine_init_clbk_t pre_init_clbk;
    tinyengine_init_clbk_t post_init_clbk;

    tinyengine_clbk_t loop_clbk;
    tinyengine_clbk_t render_clbk;
    tinyengine_clbk_t update_clbk;

} tinyengine_handle_t;

tinyengine_status_t tinyengine_init(tinyengine_handle_t* engine_handle);
tinyengine_status_t tinyengine_start(tinyengine_handle_t* engine_handle);
tinyengine_status_t tinyengine_init_renderer(
    tinyengine_handle_t* engine_handle, tinyengine_renderer_t renderer_to_use);

tinyengine_status_t tinyengine_loop(tinyengine_handle_t* engine_handle);

#endif