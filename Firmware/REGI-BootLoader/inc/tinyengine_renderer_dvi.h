#pragma once
#include "tinyengine.h"
#include "tinyengine_framebuffer.h"

#define NUM_FRAME_LINES 2
#define NUM_CHANS 3
typedef uint32_t RGB888;

class TinyEngineRendererDVI : public TinyEngineRendererI {
private:
    TinyEngineFrameBuffer& m_frame_buf;
    // TinyEngine& m_engine;
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

    enum Mode {
        MODE_PALETTE = 2,
        MODE_RGB565 = 1,
        MODE_RGB888 = 3,
        MODE_TEXT_MONO = 4,
        MODE_TEXT_RGB111 = 5,
    };

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
    TinyEngineRendererDVI(TinyEngineFrameBuffer& fr_buf);

    ~TinyEngineRendererDVI() {};
    // void dma_irq_handler();
    tinyengine_status_t swap_blocking() { return TINYENGINE_OK; };
    tinyengine_status_t swap_non_blocking() { return TINYENGINE_OK; };
    tinyengine_status_t wait_for_vsync();
    tinyengine_status_t tinyengine_renderer_init();
    void gfx_dma_handler();

protected:
    // friend void vsync_callback();
    uint16_t display_width = 320;
    uint16_t display_height = 240;
    uint16_t frame_width = 320;
    uint16_t frame_height = 240;
    uint8_t frame_bytes_per_pixel = 1;
    uint8_t h_repeat = 4;
    uint8_t v_repeat = 4;
    Mode mode = MODE_RGB565;
};

