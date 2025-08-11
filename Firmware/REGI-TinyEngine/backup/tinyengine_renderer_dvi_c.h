#pragma once
extern "C" {

#include "pico/stdlib.h"

    void tinyengine_renderer_dvi_init(uint8_t* _display_buffer, uint32_t size, uint32_t height, uint32_t width);

    //block until next vsync is hit and the buffers are flipped then the swap happens.
    void tinyengine_renderer_dvi_flip_blocking();

}