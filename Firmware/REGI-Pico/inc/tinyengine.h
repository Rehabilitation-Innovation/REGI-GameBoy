#ifndef __TINYENGINE_H__
#define __TINYENGINE_H__

#include "hardware/spi.h"
#include <stdint.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"

#define telog(M, ...) \
  printf("[Tiny Engine Log] (%s:%d)" M "\r\n", __BASE_FILE__, __LINE__, ##__VA_ARGS__)

#define teerr(M, ...) \
  printf("[Tiny Engine ERROR] (%s:%d)" M "\r\n", __BASE_FILE__, __LINE__, ##__VA_ARGS__)

typedef enum {
    TINYENGINE_OK,
    TINYENGINE_ERROR,
    TINYENGINE_RENDER_ERROR,
    TINYENGINE_OUTOFBOUNDS_ERROR
} tinyengine_status_t;

typedef enum {
    TINYENGINE_RENDERER_DVI,
    TINYENGINE_RENDERER_LCD_ST7735R,  // 1.8 in adafruit green tab with sdcard
    TINYENGINE_RENDERER_LCD_ILI9488,  // 3.5 in waveshare dev kit with sdcard
} tinyengine_renderer_t;

typedef tinyengine_status_t(*tinyengine_init_clbk_t)();


typedef void(*te_draw_pixel_func_t)(uint8_t* pixel_buffer, uint32_t len);

typedef struct {
    tinyengine_renderer_t current_output_renderer;
    te_draw_pixel_func_t set_buffer;
} tinyengine_renderer_handle_t;

// The reason this is a uint8_t* in the paramenter is that its defined before the engine handle
// but it needs access to it. So to give it access i give it a uint8_t* instead of struct*
typedef tinyengine_status_t(*tinyengine_clbk_t)(double delta);

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

    uint32_t display_w;
    uint32_t display_h;

    tinyengine_renderer_handle_t* render_engine_handle;

    tinyengine_renderer_t render_engine_to_use;

    tinyengine_init_clbk_t pre_init_clbk;
    tinyengine_init_clbk_t post_init_clbk;

    tinyengine_clbk_t loop_clbk;
    tinyengine_clbk_t render_clbk;
    tinyengine_clbk_t update_clbk;

    uint16_t fps;

} tinyengine_handle_t;

tinyengine_status_t tinyengine_init(tinyengine_handle_t* engine_handle);
tinyengine_status_t tinyengine_start(tinyengine_handle_t* engine_handle);


tinyengine_status_t tinyengine_start_loop(tinyengine_handle_t* engine_handle);

#define TE_COLOR_BLUE  0x001F  /**< Blue */
#define TE_COLOR_RED  0xF800  /**< Red */
#define TE_COLOR_GREEN  0x07E0  /**< Green */
#define TE_COLOR_CYAN  0x07FF  /**< Cyan */
#define TE_COLOR_MAGENTA  0xF81F  /**< Magenta */
#define TE_COLOR_YELLOW  0xFFE0  /**< Yellow */
#define TE_COLOR_WHITE  0xFFFF  /**< White */
#define TE_COLOR_TAN  0xED01  /**< Tan */
#define TE_COLOR_GREY  0x9CD1  /**< Grey */
#define TE_COLOR_BROWN  0x6201  /**< Brown */
#define TE_COLOR_DGREEN  0x01C0  /**< Dark Green */
#define TE_COLOR_ORANGE  0xFC00  /**< Orange */
#define TE_COLOR_NAVY  0x000F  /**< Navy */
#define TE_COLOR_DCYAN  0x03EF  /**< Dark Cyan */
#define TE_COLOR_MAROON  0x7800  /**< Maroon */
#define TE_COLOR_PURPLE  0x780F  /**< Purple */
#define TE_COLOR_OLIVE  0x7BE0  /**< Olive */
#define TE_COLOR_LGREY  0xC618  /**< Light Grey */
#define TE_COLOR_DGREY  0x7BEF  /**< Dark Grey */
#define TE_COLOR_GYELLOW  0xAFE5  /**< Greenish Yellow */
#define TE_COLOR_PINK  0xFC18  /**< Pink */
#define TE_COLOR_LBLUE  0x7E5F  /**< Light Blue */
#define TE_COLOR_BLACK  0x0000  /**< Black */
#define TE_COLOR_BEIGE  0xB5D2  /**< Beige */


#endif