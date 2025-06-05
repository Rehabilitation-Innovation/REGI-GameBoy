#ifndef __TINYENGINE_RENDERER_DVI_H__
#define __TINYENGINE_RENDERER_DVI_H__
#include "tinyengine.h"
tinyengine_status_t tinyengine_renderer_dvi_init(
    tinyengine_handle_t* engine_handle, uint8_t* buffer, uint32_t len);

void tinyengine_renderer_dvi_set_buffer(uint8_t* buffer, uint32_t len);
void wait_for_vsync();
#endif