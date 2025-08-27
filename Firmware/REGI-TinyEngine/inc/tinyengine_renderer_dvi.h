/**
 *@file tinyengine_renderer_dvi.h
 * @author A. Singh (--)
 * @brief DVI output renderer implementation for RP2350 HSTX
 * @version 1.0
 * @date 2025-08-27
 * 
 * Glenrose Rehabilitation Research and Innovation (C) 2025
 * 
 */
#pragma once
#include "tinyengine.h"
#include "tinyengine_framebuffer.h"

#define NUM_FRAME_LINES 2
#define NUM_CHANS 3
typedef uint32_t RGB888;

/**
 *@brief Implementation of the DVI HSTX renderer from Adafruit. Fixed and working for palette mode.
 * 
 */
class TinyEngineRendererDVI : public TinyEngineRendererI {
private:
    /**
     *@brief Private refrence for the display buffer.
     * 
     */
    TinyEngineFrameBuffer& m_frame_buf;
    // TinyEngine& m_engine;
    /**
     *@brief Pointers to display and back buffers used for swaping.
     * 
     */
    uint8_t* frame_buffer_display;
    uint8_t* frame_buffer_back;

    // DMA scanline filling
    uint ch_num = 0;
    int line_num = -1;

    volatile int v_scanline = 2;
    volatile bool flip_next;
    bool inited = false;

    uint32_t* line_buffers;
    int v_inactive_total;
    int v_total_active_lines;

    uint h_repeat_shift = 1;
    uint v_repeat_shift = 1;
    int line_bytes_per_pixel = 4;

    uint32_t* display_palette = nullptr;

public:

    static constexpr int PALETTE_SIZE = 256;

    RGB888 palette[PALETTE_SIZE];

    /**
     *@brief As of August 2025 only PALLETTE is implemented.
     * 
     */
    enum Mode {
        MODE_PALETTE = 2,
        MODE_RGB565 = 1,
        MODE_RGB888 = 3,
        MODE_TEXT_MONO = 4,
        MODE_TEXT_RGB111 = 5,
    };


    /**
     *@brief Unused. Should be implemented in the future.
     * 
     */
    enum TextColour {
        TEXT_BLACK = 0,
        TEXT_RED,
        TEXT_GREEN,
        TEXT_BLUE,
        TEXT_YELLOW,
        TEXT_MAGENTA,
        TEXT_CYAN,
        TEXT_WHITE,

        BG_BLACK = 0,
        BG_RED = TEXT_RED << 3,
        BG_GREEN = TEXT_GREEN << 3,
        BG_BLUE = TEXT_BLUE << 3,
        BG_YELLOW = TEXT_YELLOW << 3,
        BG_MAGENTA = TEXT_MAGENTA << 3,
        BG_CYAN = TEXT_CYAN << 3,
        BG_WHITE = TEXT_WHITE << 3,

        ATTR_NORMAL_INTEN = 0,
        ATTR_LOW_INTEN = 1 << 6,
        ATTR_V_LOW_INTEN = 1 << 7 | ATTR_LOW_INTEN,
    };

    // tinyengine_status_t tinyengine_renderer_init(TinyEngine& engine_handle, TinyEngineFrameBuffer& framebuffer);
    //block until next vsync is hit and the buffers are flipped then the swap happens.
    // void tinyengine_renderer_dvi_flip_blocking();
    /**
     *@brief Construct a new Tiny Engine Renderer D V I object
     * 
     * @param fr_buf 
     */
    TinyEngineRendererDVI(TinyEngineFrameBuffer& fr_buf);

    /**
     *@brief Destroy the Tiny Engine Renderer D V I object
     * 
     */
    ~TinyEngineRendererDVI() {};
    // void dma_irq_handler();

    /**
     *@brief Use standard library to swap local buffers for display and back.
     * 
     * @return tinyengine_status_t 
     */
    tinyengine_status_t swap_blocking() { return TINYENGINE_OK; };
    tinyengine_status_t swap_non_blocking() { return TINYENGINE_OK; };

    /**
     *@brief Wait for DVI encoder to reach VSYNC. Uses Core events, needs to be disabled to be able to debug breakpoints.
     * 
     * @return tinyengine_status_t 
     */
    tinyengine_status_t wait_for_vsync();
    tinyengine_status_t tinyengine_renderer_init();
    void gfx_dma_handler();

protected:
    // friend void vsync_callback();
    /**
     *@brief Fixed values for the current version. Should be changed appropriatly to fit future versions.
     * 
     */

    uint16_t display_width = 320;
    uint16_t display_height = 240;
    uint16_t frame_width = 320;
    uint16_t frame_height = 240;
    uint8_t frame_bytes_per_pixel = 1;
    uint8_t h_repeat = 4;
    uint8_t v_repeat = 4;
    Mode mode = MODE_RGB565;
};

