#ifndef __TINYENGINE_RENDERER_DVI_H__
#define __TINYENGINE_RENDERER_DVI_H__
#include "tinyengine.h"
#include "tinyengine_framebuffer.h"

tinyengine_status_t tinyengine_renderer_dvi_init(tinyengine_handle_t* engine_handle, te_fb_handle_t* framebuffer);

//block until next vsync is hit and the buffers are flipped then the swap happens.
void tinyengine_renderer_dvi_flip_blocking();
#endif