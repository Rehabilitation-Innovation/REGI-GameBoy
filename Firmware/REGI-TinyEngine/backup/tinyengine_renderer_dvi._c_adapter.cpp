#include "tinyengine_renderer_dvi_c_adapter.h"
tinyengine_status_t TinyEngineRendererDVI::swap_blocking() {};
tinyengine_status_t TinyEngineRendererDVI::swap_non_blocking() {};
tinyengine_status_t TinyEngineRendererDVI::wait_for_vsync() {
    tinyengine_renderer_dvi_flip_blocking();
};
tinyengine_status_t TinyEngineRendererDVI::tinyengine_renderer_init() {
    telog("Starting Renderer");
    tinyengine_renderer_dvi_init(m_frame_buf.pixel_buffer_display, m_frame_buf.m_pixel_buffer_size, 320, 240);

};