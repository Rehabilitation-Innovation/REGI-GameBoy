#include "tinyengine.h"
#include "tinyengine_renderer_st7735r.h"
#include <pico/time.h>

tinyengine_status_t tinyengine_init(tinyengine_handle_t* engine_handle)
{

    tinyengine_status_t line_status = TINYENGINE_OK;

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
    tinyengine_handle_t* engine_handle, tinyengine_renderer_t renderer_to_use) {
    tinyengine_status_t line_status = TINYENGINE_OK;

    switch (renderer_to_use) {
    case TINYENGINE_RENDERER_LCD_ST7735R:
        telog("Using 1.8 in lcd renderer");
        line_status = tinyengine_init_renderer_st7735r(engine_handle);
        if (line_status != TINYENGINE_OK) {
            teerr("Error Initializing renderer");
            return line_status;
        }
        break;

    default:
        teerr("Unsupported Renderer");
        break;
    }

    return line_status;

}

tinyengine_status_t tinyengine_loop(tinyengine_handle_t* engine_handle)
{

    double lastTime = get_absolute_time();
    double unproccessedTime = 0;
    double passedTime = 0;
    double startTime = 0;
    double frameCounter = 0;
    int render = 1;
    int frames = 0;
    float framerate = 0;
    float frametime = 0;

    while (1) { //isRunning
        telog("Rnning");
        startTime = get_absolute_time();
        passedTime = startTime - lastTime;

        lastTime = startTime;

        unproccessedTime += passedTime;

        frameCounter += passedTime;

        render = 1;

        while (1 // isRunning
            && (unproccessedTime > frametime)) {
            render = 1;


            unproccessedTime -= frametime;

            engine_handle->update_clbk(frametime);

            if (frameCounter >= 1.0) {
                telog("FPS: %d", frames);
                frameCounter = 0;
                frames = 0;
            }

        }
        if (render) {
            engine_handle->render_clbk(frametime);
            frames += 1;
        }
        else {
            // sleep here for rtos;
            // sleep_ms(1);
        }

    }


    return TINYENGINE_OK;
}
