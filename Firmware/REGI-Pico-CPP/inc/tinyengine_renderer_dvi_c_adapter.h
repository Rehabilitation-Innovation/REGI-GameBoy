#pragma once
#include "tinyengine.h"
#include "tinyengine_framebuffer.h"


#include "tinyengine_renderer_dvi_c.h"


class TinyEngineRendererDVI : public TinyEngineRendererI {
private:
    TinyEngineFrameBuffer& m_frame_buf;
    // TinyEngine& m_engine;

public:
    // tinyengine_status_t tinyengine_renderer_init(TinyEngine& engine_handle, TinyEngineFrameBuffer& framebuffer);
    //block until next vsync is hit and the buffers are flipped then the swap happens.
    // void tinyengine_renderer_dvi_flip_blocking();
    TinyEngineRendererDVI(TinyEngineFrameBuffer& fr_buf) : m_frame_buf(fr_buf) { tinyengine_renderer_init(); };

    ~TinyEngineRendererDVI() {};
    // void dma_irq_handler();
    tinyengine_status_t swap_blocking();
    tinyengine_status_t swap_non_blocking();
    tinyengine_status_t wait_for_vsync();
    tinyengine_status_t tinyengine_renderer_init();

};

