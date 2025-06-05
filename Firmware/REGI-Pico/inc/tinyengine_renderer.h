#ifndef __TINYENGINE_RENDERER_H__
#define __TINYENGINE_RENDERER_H__

#include "tinyengine.h"
#include "tinyengine_framebuffer.h"

tinyengine_status_t tinyengine_init_renderer(
    tinyengine_handle_t* engine_handle, tinyengine_renderer_t renderer_to_use, te_fb_handle_t* framebuf);

#endif