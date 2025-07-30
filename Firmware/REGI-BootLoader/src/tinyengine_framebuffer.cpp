#include "tinyengine_framebuffer.h"
#include <stdlib.h>
#include <malloc.h>
#include "main.h"
#include "memory.h"
#include "hardware/interp.h"
#include "math.h"
#include "tinyengine_sprite.h"
#include "font.h"
#include "renew_font.h"


/**
 * Improvement notes:
 *  - per pixel bounds checks have small impact in Release optimizations, but they are unneccesary if the full shape is
 *  insode the buffer. Add checks before drawing or just tell the user to be carefull; will save critical renderer time.
 *  Honestly, i dont think any one will make a game that completly saturates the renderer's capabilities. Even with
 *  full screen writes it maintains ~800 FPS.
 *  - Char rendering is a utter waste of renderer time. Do this on the second core, or dont do it all the time.
 *  Espeacially for static sentances. Most of the renderer time is wasted drawing the same thing from a look up table -
 *  instead just write the text to a smaller buffer in static flash or psram and then just render pixel to pixel instead
 *  of doing the character look up every time.
 */
#define CHAR_WIDTH 7
#define CHAR_HEIGHT 8

// This is the fractional part of a number when expressing a float as a fixed point integer.
#define UNIT_LSB 16

tinyengine_status_t TinyEngineFrameBuffer::init()
{
    // this->m_display_h = frame_height;
    // this->m_display_w = frame_width;
    // this->m_is_dual = is_dual;
    m_pixel_buffer_size = this->m_display_h * this->m_display_w * 1; // dont support color depth yet.
    pixel_buffer_back = (uint8_t*)calloc(m_pixel_buffer_size, 1);

    // all buffers must be dual in this current version, will implement single in the future if memory starts to run out at higher resolutions
    if (is_dual())
    {
        pixel_buffer_display = (uint8_t*)calloc(m_pixel_buffer_size, 1);
    }
    else
    {
        pixel_buffer_display = pixel_buffer_back;
    }
    memset(pixel_buffer_display, 0xC0, m_pixel_buffer_size);
    return TINYENGINE_OK;
}


tinyengine_status_t TinyEngineFrameBuffer::destroy()
{
    free(pixel_buffer_back);
    if (is_dual())
    {
        free(pixel_buffer_display);
    }

    return TINYENGINE_OK;
}


tinyengine_status_t TinyEngineFrameBuffer::write()
{
    return TINYENGINE_OK;
}

tinyengine_status_t TinyEngineFrameBuffer::clear(uint8_t color)
{
    memset(this->pixel_buffer_back, color, this->m_pixel_buffer_size);
    // uint16_t _x = 0, _y = 0;
    // for (_x = 0; _x < m_display_w; ++_x)
    // {
    //     for (_y = 0; _y < m_display_h; ++_y)
    //     {
    //         this->pixel_buffer_back[(_y * this->m_display_w + _x) * 1] = color;
    //         // if (_y % 10 == 0 && _x % 10 == 0)
    //         //     this->pixel_buffer_back[(_y * this->m_display_w + _x) * 1] = 15;
    //
    //
    //     }
    //
    // }

    return TINYENGINE_OK;
}

tinyengine_status_t TinyEngineFrameBuffer::draw_pixel(uint32_t x, uint32_t y, uint8_t color)
{
    //TODO: Find a better way to do this, rather than calling it all the time. this will work for now.
    // I predict this will significantly boost fps if it was implemented higher up in the draw calls.
    // Rather than checking every pixel.
    if ((x >= this->m_display_w) || (y >= this->m_display_h))
        return TINYENGINE_OUTOFBOUNDS_ERROR;
    // Write the color to the buffer
    this->pixel_buffer_back[(y * this->m_display_w + x) * 1] = (uint8_t)(color);
    // * 1 is because we store 1 byte per pixel if it was RGB888 then it would be 3


    return TINYENGINE_OK;
}

tinyengine_status_t TinyEngineFrameBuffer::draw_grid(uint32_t spacing, uint8_t color)
{
    uint16_t x = 0, y = 0;
    for (int x = 0; x < m_display_w; ++x)
    {
        for (int y = 0; y < m_display_h; ++y)
        {
            if (y % spacing == 0 && x % spacing == 0)
                this->pixel_buffer_back[(y * this->m_display_w + x) * 1] = (uint8_t)(color);
        }
    }
    return TINYENGINE_OK;
}

tinyengine_status_t TinyEngineFrameBuffer::draw_outline_rectangle(uint32_t x, uint32_t y, uint32_t w, uint32_t h,
                                                                  uint8_t color)
{
    if (!(x <= this->m_display_w && y <= this->m_display_h && x > 0 && y > 0))
    {
        for (uint32_t i = 0; i < h; i++)
            draw_pixel(x, y + i, color);
        for (uint32_t i = 0; i < h; i++)
            draw_pixel(x + w, y + i, color);

        for (uint32_t i = 0; i < w; i++)
            draw_pixel(x + i, y, color);
        for (uint32_t i = 0; i < w; i++)
            draw_pixel(x + i, y + h, color);
    }
    else // this is just easier to write for now, u can make a new function called draw_pixel_unsafe or something...
    {
        // draw sides
        for (uint16_t i = 0; i < h; i++)
            this->pixel_buffer_back[((y + i) * this->m_display_w + (x)) * 1] = (uint8_t)(color);
        for (uint16_t i = 0; i < h; i++)
            this->pixel_buffer_back[((y + i) * this->m_display_w + (x + w)) * 1] = (uint8_t)(color);

        // draw top and bottom; Added 200 fps lol
        // for (uint32_t i = 0; i < w; i++)
        memset(pixel_buffer_back + ((y) * this->m_display_w + (x)) * 1, color, w);
        // this->pixel_buffer_back[((y) * this->m_display_w + (x + i)) * 1] = (uint8_t)(color);
        // for (uint32_t i = 0; i < w; i++)
        memset(pixel_buffer_back + ((y + h) * this->m_display_w + (x)) * 1, color, w);
        // this->pixel_buffer_back[((y + h) * this->m_display_w + (x + i)) * 1] = (uint8_t)(color);
    }

    return TINYENGINE_OK;
}

tinyengine_status_t TinyEngineFrameBuffer::draw_filled_rectangle(uint32_t x, uint32_t y, uint32_t w, uint32_t h,
                                                                 uint8_t color)
{
    // uint8_t* temp = malloc(sizeof(uint8_t) * this.m_pixel_buffer_size);
    // memset(temp, color, this.m_pixel_buffer_size);

    // if (!(x <= this->m_display_w && y <= this->m_display_h && x > 0 && y > 0))
    // {
    //     for (uint16_t i = 0; i < h; i++)
    //         for (uint16_t j = 0; j < w; j++)
    //             //this->pixel_buffer[j + i * this->display_w] = color;
    //             draw_pixel(x + j, y + i, color);
    // }
    // else
    // {
    for (uint16_t i = 0; i < h; i++)
        // for (uint16_t j = 0; j < w; j++)
        //this->pixel_buffer[j + i * this->display_w] = color;
        // draw_pixel(x + j, y + i, color);
        memset(this->pixel_buffer_back + (y + i) * this->m_display_w + (x), color, w);
    // this->pixel_buffer_back[((y + i) * this->m_display_w + (x + j)) * 1] = (uint8_t)(color);
    // }
    // {
    // }
    return TINYENGINE_OK;
}

tinyengine_status_t TinyEngineFrameBuffer::draw_outline_circle(uint32_t x, uint32_t y, uint32_t r, uint8_t color)
{
    return TINYENGINE_OK;
}

// https://www.geeksforgeeks.org/bresenhams-circle-drawing-algorithm/

void TinyEngineFrameBuffer::drawCircle1(int xc, int yc, int x, int y, uint8_t color)
{
    draw_pixel(xc + x, yc + y, color);
    draw_pixel(xc - x, yc + y, color);
    draw_pixel(xc + x, yc - y, color);
    draw_pixel(xc - x, yc - y, color);
    draw_pixel(xc + y, yc + x, color);
    draw_pixel(xc - y, yc + x, color);
    draw_pixel(xc + y, yc - x, color);
    draw_pixel(xc - y, yc - x, color);
}

tinyengine_status_t TinyEngineFrameBuffer::draw_filled_circle(uint32_t xc, uint32_t yc, uint32_t r, uint8_t color)
{
    int x = 0, y = r;
    int d = 3 - 2 * r;
    drawCircle1(xc, yc, x, y, color);
    while (y >= x)
    {
        // check for decision parameter
        // and correspondingly
        // update d, y
        if (d > 0)
        {
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

tinyengine_status_t TinyEngineFrameBuffer::swap_blocking()
{
    // this->wait_vsync_blocking_func_ptr();
    uint8_t* frame_buffer_temp = this->pixel_buffer_back;
    // display->flip_now();
    this->pixel_buffer_back = this->pixel_buffer_display;
    this->pixel_buffer_display = frame_buffer_temp;

    // std::swap(pixel_buffer_display, pixel_buffer_back);

    return TINYENGINE_OK;
}

tinyengine_status_t TinyEngineFrameBuffer::draw_sprite(Sprite& sprite)
{
    te_sprite_t* data = sprite.get_m_sprite_data();

    if (!sprite.is_m_animated())
    {
        return draw_sprite_raw(
            data->sprite_buffer,
            data->width,
            data->height,
            sprite.get_m_x(),
            sprite.get_m_y());
    }

    te_sprite_animation_t* ani = sprite.get_m_animation_data();

    if (sprite.get_m_frametime() >= (float)ani->animation_delay)
    {
        ani->current_frame += 1;
        // ani->current_frame_time = 0;
        sprite.set_m_frametime(0);
    }

    if (ani->current_frame > ani->total_frames - 1
        || ani->current_frame >= ani->end_frame)
    {
        ani->current_frame = ani->start_frame;
    }

    // ani->current_frame_time += sprite.get_m_frametime() * 100;
    // sprite.set_m_frametime(sprite.get_m_frametime() * sprite.get_m_animation_speed());
    return draw_sprite_raw(ani->sprite_animation_frames[ani->current_frame],
                           data->width,
                           data->height,
                           sprite.get_m_x(),
                           sprite.get_m_y());
}

tinyengine_status_t TinyEngineFrameBuffer::draw_sprite_raw(
    uint8_t* sprite, uint32_t w, uint32_t h, uint32_t x, uint32_t y)
{
    // uint16_t _x, _y;
    // for (uint32_t i = 0; i < h * w; i++)
    // {
    //     _x = i / h;
    //     _y = i % w;
    //     // if (_x == 0 && _y == 0) {
    //     //     draw_pixel(x, y, 0xC0);
    //     //     continue;
    //     // }
    //     if (sprite[i] != 0x00)
    //         draw_pixel(_y + x, _x + y, sprite[i]);
    // }

    // added 400 fps lol idk how, maybe i am wrong lmao
    for (int i = 0; i < h; ++i)
    {
        memcpy(pixel_buffer_back + (y + i) * this->m_display_w + (x), sprite + i * w, w);
    }

    return TINYENGINE_OK;
}

tinyengine_status_t TinyEngineFrameBuffer::draw_sprite_raw_batch(uint8_t* sprite, uint32_t w, uint32_t h, uint16_t x[],
                                                                 uint16_t y[], uint16_t count)
{
    uint16_t _x, _y;
    for (uint16_t i = 0; i < h * w; i++)
    {
        _x = i / h;
        _y = i % w;
        if (sprite[i] != 0x00)
            for (uint8_t j = 0; j < count; j++)
            {
                draw_pixel(x[j] - _y, y[j] + _x, sprite[i]);
            }
    }


    return TINYENGINE_OK;
}

tinyengine_status_t TinyEngineFrameBuffer::draw_char(char _char, uint32_t x, uint32_t y, uint8_t _color)
{
    // Convert the character to an index
    _char = _char & 0x7F;
    if (_char < ' ')
    {
        _char = 0;
    }
    else
    {
        _char -= ' ';
    }
    // 'font' is a multidimensional array of [96][char_width]
    // which is really just a 1D array of size 96*char_width.
    const uint8_t* chr = renew_font[_char];
    // Draw pixels
    for (uint8_t j = 0; j < CHAR_WIDTH; j++)
    {
        for (uint8_t i = 0; i < CHAR_HEIGHT; i++)
        {
            if (chr[j] & (1 << i))
            {
                // this added like 100FPS to the game on full optimizations
                this->pixel_buffer_back[((y + i) * this->m_display_w + (x + j)) * 1] = (uint8_t)(_color);
                // draw_pixel(x + j, y + i, _color);
            }
        }
    }
    return TINYENGINE_OK;
}

tinyengine_status_t TinyEngineFrameBuffer::draw_string(const char* _string, uint32_t x, uint32_t y, uint8_t _color)
{
    while (*_string)
    {
        draw_char(*_string++, x, y, _color);
        x += CHAR_WIDTH;
    }
    return TINYENGINE_OK;
}

tinyengine_status_t TinyEngineFrameBuffer::copy_buffer_slow(TinyEngineFrameBuffer& _buffer_to_copy, uint16_t _dst_x,
                                                            uint16_t _dst_y)
{
    for (int i = 0; i < _buffer_to_copy.m_display_h; ++i)
    {
        for (int j = 0; j < _buffer_to_copy.m_display_w; ++j)
        {
            this->pixel_buffer_back[((_dst_y + i) * this->m_display_w + (_dst_x + j)) * 1] = _buffer_to_copy.
                pixel_buffer_back[i * _buffer_to_copy.m_display_w + j];
        }
    }
    return TINYENGINE_OK;
}
