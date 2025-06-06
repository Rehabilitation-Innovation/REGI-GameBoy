#include "tinyengine.h"
#include "tinyengine_renderer_st7735r.h"
#include "tinyengine_renderer_dvi.h"
#include "tinyengine_renderer_ili9488.h"
#include <pico/time.h>
#include "tinyengine_framebuffer.h"


tinyengine_status_t tinyengine_init(tinyengine_handle_t* engine_handle)
{

    tinyengine_status_t line_status = TINYENGINE_OK;
    engine_handle->fps = 0;
    line_status = engine_handle->pre_init_clbk();
    if (line_status != TINYENGINE_OK) {
        return line_status;
    }

    line_status = engine_handle->post_init_clbk();
    if (line_status != TINYENGINE_OK) {
        return line_status;
    }

    return TINYENGINE_OK;
}

tinyengine_status_t tinyengine_start(tinyengine_handle_t* engine_handle)
{

    return TINYENGINE_OK;
}

tinyengine_status_t tinyengine_init_renderer(
    tinyengine_handle_t* engine_handle, tinyengine_renderer_t renderer_to_use, te_fb_handle_t* framebuf) {
    tinyengine_status_t line_status = TINYENGINE_OK;

    engine_handle->render_engine_handle->current_output_renderer = renderer_to_use;

    switch (renderer_to_use) {
    case TINYENGINE_RENDERER_LCD_ST7735R:
        telog("Using 1.8 in lcd renderer");
        line_status = tinyengine_renderer_st7735r_init(engine_handle);
        if (line_status != TINYENGINE_OK) {
            teerr("Error Initializing ST7735R renderer");
            return line_status;
        }
        // engine_handle->render_engine_handle->set_buffer = tiny


        break;

    case TINYENGINE_RENDERER_DVI:
        telog("Using DVI Renderer");
        // tinyengine_renderer_dvi_set_buffer(framebuf, size);
        line_status = tinyengine_renderer_dvi_init(engine_handle, framebuf);
        if (line_status != TINYENGINE_OK) {
            teerr("Error Initializing DVI renderer");
            return line_status;
        }
        framebuf->te_fb_wait_vsync_blocking_func_ptr = tinyengine_renderer_dvi_flip_blocking;
        break;

    case TINYENGINE_RENDERER_LCD_ILI9488:
        telog("Using ILI9488 Renderer");
        line_status = tinyengine_renderer_ili9488_init(engine_handle);
        if (line_status != TINYENGINE_OK) {
            teerr("Error Initializing ILI9488 renderer");
            return line_status;
        }
        break;

    default:
        teerr("Unsupported Renderer");
        break;
    }

    return line_status;

}

void tinyengine_render(tinyengine_handle_t* engine_handle, double frametime) {
    // telog("Rendering");
    // wait_for_vsync();
    engine_handle->render_clbk(frametime);

}

void tinyengine_update(tinyengine_handle_t* engine_handle, double frametime) {
    engine_handle->update_clbk(frametime);
}

tinyengine_status_t tinyengine_start_loop(tinyengine_handle_t* engine_handle)
{

    double lastTime = ((double)to_ms_since_boot(get_absolute_time())) / ((double)1000);
    double unproccessedTime = 0;
    double passedTime = 0;
    double startTime = 0;
    int render = 1;
    int frames = 0;
    int framerate = 60;
    double frametime = (double)1 / (double)framerate;

    uint32_t startus = to_us_since_boot(get_absolute_time());
    double frameCounter = 0;

    while (1) { //isRunning

        startTime = ((double)to_ms_since_boot(get_absolute_time())) / ((double)1000);
        passedTime = startTime - lastTime;

        lastTime = startTime;

        unproccessedTime += passedTime;

        frameCounter += passedTime;

        render = 0; // run uncapped

        while ((unproccessedTime > frametime)) {
            render = 1;

            // telog("Running");
            unproccessedTime -= frametime;

            // engine_handle->update_clbk(frametime);

            tinyengine_update(engine_handle, frametime);

            if (frameCounter >= 1) {
                telog("FPS: %d", frames);
                frameCounter = 0;
                frames = 0;
            }

        }
        if (render) {
            // engine_handle->render_clbk(frametime);
            tinyengine_render(engine_handle, frametime);
            frames += 1;
        }
        else {
            // sleep here for rtos;
            // sleep_ms(1);
        }

    }


    return TINYENGINE_OK;
}
