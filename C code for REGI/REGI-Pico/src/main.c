#include "main.h"

#include <stdio.h>

#include "hardware/spi.h"

#include "pico/stdlib.h"

#include "tinyengine.h"

tinyengine_status_t post_init() {
}

tinyengine_status_t pre_init() {
}

tinyengine_status_t update(float frameTime) {
}

tinyengine_status_t render(float frameTime) {
}

tinyengine_status_t loop_clbk(float frameTime) {
}


int main() {
    stdio_init_all();

    tinyengine_handle_t engine = {
    .lcd_spi_bklt = LCD_BKLT,
    .lcd_spi_clk = LCD_SPI_CLK,
    .lcd_spi_cs = LCD_CS,
    .lcd_spi_dc = LCD_DC,
    .lcd_spi_instance = LCD_SPI_PORT,
    .lcd_spi_miso = LCD_SPI_MISO,
    .lcd_spi_mosi = LCD_SPI_MOSI,
    .lcd_spi_speed = LCD_SPI_SPEED,
    .lcd_spi_rst = LCD_RST,
    .pre_init_clbk = pre_init,
    .post_init_clbk = post_init,
    .update_clbk = update,
    .render_clbk = render,
    .loop_clbk = loop_clbk
    };


    tinyengine_init(&engine);

    tinyengine_init_renderer(&engine, TINYENGINE_RENDERER_LCD_ST7735R);

    tinyengine_start(&engine);



    // maybe shut down and reboot after?

    while (1) {
        tinyengine_loop(&engine);
    }
    return 0;
}
