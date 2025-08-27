/**
 *@file tinyengine_framebuffer.h
 * @author A. Singh (--)
 * @brief Memory mapped framebuffer. This class contains 1D arrays for rendering 2D graphics. Also implements double buffering and swaping.
 * @version 1.0
 * @date 2025-08-27
 * 
 * Glenrose Rehabilitation Research and Innovation (C) 2025
 * 
 */
#pragma once
#include "tinyengine.h"
#ifndef PIXEL_COLOR_DEPTH_BITS
#define PIXEL_COLOR_DEPTH_BITS 16
#endif

// @TODO: add presets for resolutions, for now just use 320x240x2bpp ; RBG555
// typedef void(*swap_blocking_func_t)();
/**
 * No need to edit this struct init will return a filled struct.
 */
 // typedef struct {
/**
 *@brief Framebuffer class for rendering primitives and sprites. Contains no scaling yet.
 * 
 */
class TinyEngineFrameBuffer
{
private:
    /**
     *@brief Local copy of the display dimentions.
     * 
     */
    uint32_t m_display_w;
    uint32_t m_display_h;
    /**
     *@brief Is double buffered.
     * 
     */
    int m_is_dual;

    int copy_dma_channel = -1;

    /**
     *@brief Creates memory regions and initializes buffers. Called at creation in constructor.
     * 
     * @return tinyengine_status_t 
     */
    tinyengine_status_t init();

public:
    /**
     *@brief All user render calls are thrown to this buffer. It is not rendered to screen.
     * 
     */
    uint8_t* pixel_buffer_back;
    /**
     *@brief Immutable display buffer, is swaped at vsync or every frame.
     * 
     */
    uint8_t* pixel_buffer_display;
    uint32_t m_pixel_buffer_size; // a quicker look up. rather than calculating every time; includes color depth

    bool is_dual() { return m_is_dual; };

    void drawCircle1(int xc, int yc, int x, int y, uint8_t color);

    /**
     *@brief Construct a new Tiny Engine Frame Buffer object
     * 
     * @param frame_width 
     * @param frame_height 
     * @param is_dual 
     */
    TinyEngineFrameBuffer(uint32_t frame_width, uint32_t frame_height, int is_dual) : m_display_h(frame_height),
        m_display_w(frame_width), m_is_dual(is_dual) { init(); };

    /**
     *@brief Destroy the Tiny Engine Frame Buffer object
     * 
     */
    ~TinyEngineFrameBuffer()
    {
    };

    /**
     *@brief Destroys and frees memory.
     * 
     * @return tinyengine_status_t 
     */
    tinyengine_status_t destroy();

    /**
     *@brief Legacy
     * 
     * @return tinyengine_status_t 
     */
    tinyengine_status_t write();

    /**
     *@brief Clears back buffer to given color.
     * 
     * @param color 
     * @return tinyengine_status_t 
     */
    tinyengine_status_t clear(const uint8_t color);

    /**
     *@brief Clears back buffer to given data. For example a arbitrary frambuffer sized array containing custom background.
     * 
     * @param data 
     * @return tinyengine_status_t 
     */
    tinyengine_status_t clear_with(const uint8_t* data);

    tinyengine_status_t draw_pixel(const uint32_t x, const uint32_t y, const uint8_t color);

    tinyengine_status_t draw_grid(const uint32_t spacing, const uint8_t color);

    tinyengine_status_t draw_outline_rectangle(const uint32_t x, const uint32_t y, const uint32_t w, const uint32_t h,
                                               const uint8_t color);

    tinyengine_status_t draw_filled_rectangle(const uint32_t x, const uint32_t y, const uint32_t w, const uint32_t h,
                                              const uint8_t color);

    tinyengine_status_t draw_outline_circle(const uint32_t x, const uint32_t y, const uint32_t r, const uint8_t color);

    tinyengine_status_t draw_filled_circle(const uint32_t x, const uint32_t y, const uint32_t r, const uint8_t color);

    tinyengine_status_t swap_blocking();

    tinyengine_status_t draw_sprite(Sprite& sprite);

    /**
     *@brief Draw using raw buffer.
     * 
     * @param sprite 1bpp array of sprite data.
     * @param w width of sprite.
     * @param h height of sprite
     * @param x location on buffer must be within buffer bounds. OW it is ignored.
     * @param y location on buffer must be within buffer bounds. OW it is ignored.
     * @return tinyengine_status_t 
     */
    tinyengine_status_t draw_sprite_raw(const uint8_t* sprite, const uint32_t w, const uint32_t h, const uint32_t x,
                                        const uint32_t y);

    /**
     *@brief Draw the same sprite in a single call to multiple locations on the screen.
     *@todo Implement the hardware interpolator to calculate memory addresses. This will be the perfect use for single cycle math.
     *
     * @param sprite 1bpp array of sprite data.
     * @param w width of sprite.
     * @param h height of sprite
     * @param x array of x locations. Please make sure all within bounds of the buffer. Must be indexed the same as ys
     * @param y array of y locations. Please make sure all within bounds of the buffer.
     * @param count 
     * @return tinyengine_status_t 
     */
    tinyengine_status_t draw_sprite_raw_batch(const uint8_t* sprite, const uint32_t w, const uint32_t h,
                                              const uint16_t x[], const uint16_t y[],
                                              const uint16_t count);

    /**
     *@brief Draw singular char using the global font buffer.
     * 
     * @param _char Character to draw
     * @param x location x
     * @param y location y
     * @param _color color of sprite from pallette
     * @return tinyengine_status_t 
     */
    tinyengine_status_t draw_char(const char _char, const uint32_t x, const uint32_t y, const uint8_t _color);

    /**
     *@brief Draw a string of characters using @fn draw_char. This draws mono spaced fonts the best.
     * @todo Implement other fonts and better spacing. Optimize using hardware interpolator. Do not call draw char for every character. Calculate all addresses at once.
     *
     * @param _string 1byte per element pointer for a string. Use .c_str() if you are using Cpp Strings.
     * @param x 
     * @param y 
     * @param _color 
     * @return tinyengine_status_t 
     */
    tinyengine_status_t draw_string(const char* _string, const uint32_t x, const uint32_t y, const uint8_t _color);

    /**
     *@brief Unused.
     * 
     * @param _buffer_to_copy 
     * @param _dst_x 
     * @param _dst_y 
     * @return tinyengine_status_t 
     */
    tinyengine_status_t copy_buffer_slow(const TinyEngineFrameBuffer& _buffer_to_copy, const uint16_t _dst_x,
                                         const uint16_t _dst_y);
};
