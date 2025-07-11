#include "tinyengine_framebuffer.h"
#include <stdlib.h>
#include <malloc.h>
#include "main.h"
#include "memory.h"
#include "hardware/interp.h"
#include "math.h"

tinyengine_status_t TinyEngineFrameBuffer::init() {

    // this->m_display_h = frame_height;
    // this->m_display_w = frame_width;
    // this->m_is_dual = is_dual;
    m_pixel_buffer_size = this->m_display_h * this->m_display_w * 1; // dont support color depth yet.
    pixel_buffer_back = (uint8_t*)calloc(m_pixel_buffer_size, 1);

    // all buffers must be dual in this current version, will implement single in the future if memory starts to run out at higher resolutions
    if (is_dual()) {
        pixel_buffer_display = (uint8_t*)calloc(m_pixel_buffer_size, 1);
    }
    else {
        pixel_buffer_display = pixel_buffer_back;
    }
    memset(pixel_buffer_display, 0xC0, m_pixel_buffer_size);
    return TINYENGINE_OK;
}

tinyengine_status_t TinyEngineFrameBuffer::destroy() {

    free(pixel_buffer_back);
    if (is_dual()) {
        free(pixel_buffer_display);
    }

    return TINYENGINE_OK;
}


tinyengine_status_t TinyEngineFrameBuffer::write() {

    // unused for now. will implement in future when new screen gets configured.

    // dw about buffer initialization here, trust the use calls after init; be smart dude

    // setAddrWindow(0, 0, this->display_w - 1, this->display_h);

    // tft_dc_high();
    // tft_cs_low();

    // spi_write_blocking(spi1, this->pixel_buffer, this->pixel_buffer_size);

    // tft_cs_high();

    // this->pixel_buffer = (which_buffer) ? frame_buffer : display_buffer;
    // this->pixel_buffer = (!which_buffer) ? frame_buffer : display_buffer;

    return TINYENGINE_OK;
}

tinyengine_status_t TinyEngineFrameBuffer::clear(uint8_t color) {
    // for (size_t i = 0; i < this->pixel_buffer_size; i += 2)
    // {
    //     this->pixel_buffer[i] = color >> 8;
    //     this->pixel_buffer[i + 1] = color;
    // }
    // for (uint32_t i = 0; i < this->pixel_buffer_size; i++)
    // {
    //     this->pixel_buffer_back[i] = color;
    // }
    memset(this->pixel_buffer_back, color, this->m_pixel_buffer_size);
    return TINYENGINE_OK;
}

tinyengine_status_t TinyEngineFrameBuffer::draw_pixel(uint32_t x, uint32_t y, uint8_t color) {
    if ((x >= this->m_display_w) || (y >= this->m_display_h))
        return TINYENGINE_OUTOFBOUNDS_ERROR;

    // Calculate the index in the buffer
    // size_t index = (y * this->display_w + x) * 2; // 2 bytes per pixel for RGB565
    // // // Write the color to the buffer
    // this->pixel_buffer[index] = (uint8_t)(color >> 8);     // High byte
    // this->pixel_buffer[index + 1] = (uint8_t)(color & 0xFF); // Low byte

    size_t index = (y * this->m_display_w + x) * 1;
    // Write the color to the buffer
    this->pixel_buffer_back[index] = (uint8_t)(color);


    return TINYENGINE_OK;
}

tinyengine_status_t TinyEngineFrameBuffer::draw_outline_rectangle(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t color) {
    return TINYENGINE_OK;
}

tinyengine_status_t TinyEngineFrameBuffer::draw_filled_rectangle(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t color) {

    for (uint32_t i = 0; i < h; i++)
        for (uint32_t j = 0; j < w; j++)
            //this->pixel_buffer[j + i * this->display_w] = color;
            draw_pixel(x + j, y + i, color);

    return TINYENGINE_OK;
}

tinyengine_status_t TinyEngineFrameBuffer::draw_outline_circle(uint32_t x, uint32_t y, uint32_t r, uint8_t color) {
    return TINYENGINE_OK;
}

// https://www.geeksforgeeks.org/bresenhams-circle-drawing-algorithm/

void TinyEngineFrameBuffer::drawCircle1(int xc, int yc, int x, int y, uint8_t color) {
    draw_pixel(xc + x, yc + y, color);
    draw_pixel(xc - x, yc + y, color);
    draw_pixel(xc + x, yc - y, color);
    draw_pixel(xc - x, yc - y, color);
    draw_pixel(xc + y, yc + x, color);
    draw_pixel(xc - y, yc + x, color);
    draw_pixel(xc + y, yc - x, color);
    draw_pixel(xc - y, yc - x, color);
}

tinyengine_status_t TinyEngineFrameBuffer::draw_filled_circle(uint32_t xc, uint32_t yc, uint32_t r, uint8_t color) {

    int x = 0, y = r;
    int d = 3 - 2 * r;
    drawCircle1(xc, yc, x, y, color);
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
        drawCircle1(xc, yc, x, y, color);
    }

    return TINYENGINE_OK;
}

tinyengine_status_t TinyEngineFrameBuffer::swap_blocking() {
    // this->wait_vsync_blocking_func_ptr();
    uint8_t* frame_buffer_temp = this->pixel_buffer_back;
    // display->flip_now();
    this->pixel_buffer_back = this->pixel_buffer_display;
    this->pixel_buffer_display = frame_buffer_temp;
    return TINYENGINE_OK;
}

tinyengine_status_t TinyEngineFrameBuffer::draw_sprite(te_sprite_t* sprite, uint32_t x, uint32_t y) {
    uint16_t _x, _y;
    for (uint32_t i = 0; i < sprite->height * sprite->width; i++) {
        _x = i / sprite->height;
        _y = i % sprite->width;
        if (_x == 0 && _y == 0) {
            draw_pixel(x, y, 0xC0);
            continue;
        }
        if (sprite->sprite_buffer[i] != 0x00)
            draw_pixel(_y + x, _x + y, sprite->sprite_buffer[i]);
    }

    return TINYENGINE_OK;
}

tinyengine_status_t TinyEngineFrameBuffer::draw_sprite_raw(uint8_t* sprite, uint32_t w, uint32_t h, uint32_t x, uint32_t y) {
    uint16_t _x, _y;
    for (uint32_t i = 0; i < h * w; i++) {
        _x = i / h;
        _y = i % w;
        if (_x == 0 && _y == 0) {
            draw_pixel(x, y, 0xC0);
            continue;
        }
        if (sprite[i] != 0x00)
            draw_pixel(_y + x, _x + y, sprite[i]);
    }
    return TINYENGINE_OK;
}

tinyengine_status_t TinyEngineFrameBuffer::draw_sprite_batch(te_sprite_t* sprite, uint16_t x[], uint16_t y[], uint16_t count) {

    uint16_t _x, _y;
    for (uint16_t i = 0; i < sprite->height * sprite->width; i++) {
        _x = i / sprite->height;
        _y = i % sprite->width;
        if (sprite->sprite_buffer[i] != 0x00)
            for (uint8_t j = 0; j < count; j++) {
                draw_pixel(x[j] - _y, y[j] + _x, sprite->sprite_buffer[i]);
            }
    }


    return TINYENGINE_OK;
}