#include "tinyengine.h"
#include "tinyengine_renderer_st7735r.h"

#include "hw.h"
#include "ST7735_TFT.h"

tinyengine_status_t tinyengine_init_renderer_st7735r(
    tinyengine_handle_t* engine_handle) {
    spi_init(engine_handle->lcd_spi_instance, engine_handle->lcd_spi_speed);  // SPI with 1Mhz
    gpio_set_function(engine_handle->lcd_spi_miso, GPIO_FUNC_SPI);
    gpio_set_function(engine_handle->lcd_spi_clk, GPIO_FUNC_SPI);
    gpio_set_function(engine_handle->lcd_spi_mosi, GPIO_FUNC_SPI);
    // tft_spi_init();

    gpio_init(engine_handle->lcd_spi_cs);
    gpio_set_dir(engine_handle->lcd_spi_cs, GPIO_OUT);
    gpio_put(engine_handle->lcd_spi_cs, 1);  // Chip select is active-low

    gpio_init(engine_handle->lcd_spi_dc);
    gpio_set_dir(engine_handle->lcd_spi_dc, GPIO_OUT);
    gpio_put(engine_handle->lcd_spi_dc, 0);  // Chip select is active-low

    gpio_init(engine_handle->lcd_spi_rst);
    gpio_set_dir(engine_handle->lcd_spi_rst, GPIO_OUT);
    gpio_put(engine_handle->lcd_spi_rst, 0);
}