#include "tinyengine_framebuffer.h"
#include <stdlib.h>
#include <malloc.h>
#include "ST7735_TFT.h"
#include "hw.h"
#include "main.h"

// uint32_t getTotalHeap(void) {
//     extern char __StackLimit, __bss_end__;

//     return &__StackLimit - &__bss_end__;
// }

// uint32_t getFreeHeap(void) {
//     struct mallinfo m = mallinfo();

//     return getTotalHeap() - m.uordblks;
// }

// volatile static  uint8_t display_buffer[640 * 480 * 1] = { 0 };

volatile static int which_buffer = 0; // when 0 pixel_buffer = displaybufer, when 1 pixel buffer = framebuffer

tinyengine_status_t te_fb_init(te_fb_handle_t* fb_handle, tinyengine_handle_t* engine) {

    fb_handle->display_h = engine->display_h;
    fb_handle->display_w = engine->display_w;

    fb_handle->pixel_buffer_size = fb_handle->display_h * fb_handle->display_w * 1; // dont support color depth yet.

    // allocate 2 bytes per pixel so we have RGB565 so 16 bit color depth. allocate 1 byte since our iterator(pointer) is of type uint8_t(1 byte.)
    fb_handle->pixel_buffer = calloc(fb_handle->pixel_buffer_size, 1);
    // telog("Frame Buffer Allocated; Size: %d Bytes", fb_handle->pixel_buffer_size);
    // fb_handle->pixel_buffer = display_buffer;
    // fb_handle->pixel_buffer_back = frame_buffer;
    // te_fb_clear(fb_handle, 0xE0);
    for (uint32_t i = 0; i < fb_handle->pixel_buffer_size; ++i)
    {
        fb_handle->pixel_buffer[i] = (i % 2) ? 0xE0 : 0xF0; // reed and yellow columns to indicate buffer init
    }

    // telog("Free Heap %d Bytes", getFreeHeap());

    // engine->render_engine_handle->set_buffer(fb_handle->pixel_buffer, fb_handle->pixel_buffer_size);
}

tinyengine_status_t te_fb_destroy(te_fb_handle_t* fb_handle) {
    free(fb_handle->pixel_buffer);
}


tinyengine_status_t te_fb_write(te_fb_handle_t* fb_handle) {

    // dw about buffer initialization here, trust the use calls after init; be smart dude

    // setAddrWindow(0, 0, fb_handle->display_w - 1, fb_handle->display_h);

    // tft_dc_high();
    // tft_cs_low();

    // spi_write_blocking(spi1, fb_handle->pixel_buffer, fb_handle->pixel_buffer_size);

    // tft_cs_high();

    // fb_handle->pixel_buffer = (which_buffer) ? frame_buffer : display_buffer;
    // fb_handle->pixel_buffer = (!which_buffer) ? frame_buffer : display_buffer;

    return TINYENGINE_OK;
}

tinyengine_status_t te_fb_clear(te_fb_handle_t* fb_handle, uint16_t color) {
    // for (size_t i = 0; i < fb_handle->pixel_buffer_size; i += 2)
    // {
    //     fb_handle->pixel_buffer[i] = color >> 8;
    //     fb_handle->pixel_buffer[i + 1] = color;
    // }
    for (size_t i = 0; i < fb_handle->pixel_buffer_size; i += 1)
    {
        fb_handle->pixel_buffer[i] = color;
    }
    return TINYENGINE_OK;
}

tinyengine_status_t te_fb_draw_pixel(te_fb_handle_t* fb_handle, uint32_t x, uint32_t y, uint16_t color) {
    if ((x >= fb_handle->display_w) || (y >= fb_handle->display_h))
        return TINYENGINE_OUTOFBOUNDS_ERROR;

    // Calculate the index in the buffer
    // size_t index = (y * fb_handle->display_w + x) * 2; // 2 bytes per pixel for RGB565
    // // // Write the color to the buffer
    // fb_handle->pixel_buffer[index] = (uint8_t)(color >> 8);     // High byte
    // fb_handle->pixel_buffer[index + 1] = (uint8_t)(color & 0xFF); // Low byte

    size_t index = (y * fb_handle->display_w + x) * 1; // 2 bytes per pixel for RGB332
    // Write the color to the buffer
    fb_handle->pixel_buffer[index] = (uint8_t)(color) & 0xFF;


    return TINYENGINE_OK;
}

tinyengine_status_t te_fb_draw_outline_rectangle(te_fb_handle_t* fb_handle, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint16_t color) {
    return TINYENGINE_OK;
}

tinyengine_status_t te_fb_draw_filled_rectangle(te_fb_handle_t* fb_handle, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint16_t color) {

    for (uint32_t i = 0; i < h; i++)
        for (uint32_t j = 0; j < w; j++)
            //fb_handle->pixel_buffer[j + i * fb_handle->display_w] = color;
            te_fb_draw_pixel(fb_handle, x + j, y + i, color);

    return TINYENGINE_OK;
}

tinyengine_status_t te_fb_draw_outline_circle(te_fb_handle_t* fb_handle, uint32_t x, uint32_t y, uint32_t r, uint16_t color) {
    return TINYENGINE_OK;
}

// https://www.geeksforgeeks.org/bresenhams-circle-drawing-algorithm/

void drawCircle1(te_fb_handle_t* fb_handle, int xc, int yc, int x, int y, uint16_t color) {
    te_fb_draw_pixel(fb_handle, xc + x, yc + y, color);
    te_fb_draw_pixel(fb_handle, xc - x, yc + y, color);
    te_fb_draw_pixel(fb_handle, xc + x, yc - y, color);
    te_fb_draw_pixel(fb_handle, xc - x, yc - y, color);
    te_fb_draw_pixel(fb_handle, xc + y, yc + x, color);
    te_fb_draw_pixel(fb_handle, xc - y, yc + x, color);
    te_fb_draw_pixel(fb_handle, xc + y, yc - x, color);
    te_fb_draw_pixel(fb_handle, xc - y, yc - x, color);
}

tinyengine_status_t te_fb_draw_filled_circle(te_fb_handle_t* fb_handle, uint32_t xc, uint32_t yc, uint32_t r, uint16_t color) {

    int x = 0, y = r;
    int d = 3 - 2 * r;
    drawCircle1(fb_handle, xc, yc, x, y, color);
    while (y >= x) {

        // check for decision parameter
        // and correspondingly 
        // update d, y
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        }
        else
            d = d + 4 * x + 6;

        // Increment x after updating decision parameter
        x++;

        // Draw the circle using the new coordinates
        drawCircle1(fb_handle, xc, yc, x, y, color);
    }

    return TINYENGINE_OK;
}
