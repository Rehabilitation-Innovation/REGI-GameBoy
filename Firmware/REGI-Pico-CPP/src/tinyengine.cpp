#include "tinyengine.h"
// #include "tinyengine_renderer_st7735r.h"
#include "tinyengine_renderer_dvi.h"
// #include "tinyengine_renderer_ili9488.h"
#include <pico/time.h>
#include "tinyengine_framebuffer.h"

tinyengine_status_t TinyEngine::init()
{
    pre_init_clbk();
    return TINYENGINE_OK;
}

tinyengine_status_t TinyEngine::start()
{
    return TINYENGINE_OK;
}

void TinyEngine::render(double frametime) {
}

void TinyEngine::update(double frametime) {
    // engine_handle->update_clbk(frametime);
}

tinyengine_status_t TinyEngine::start_loop()
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
            // tinyengine_update(engine_handle, frametime);
            update_clbk(frametime);
            if (frameCounter >= 1) {
                telog("FPS: %d", frames);
                frameCounter = 0;
                frames = 0;
            }
        }
        if (render) {
            // engine_handle->render_clbk(frametime);
            // tinyengine_render(engine_handle, frametime);
            render_clbk();
            frames += 1;
        }
        else {
            // sleep here for rtos;
            // sleep_ms(1);
        }
    }
    return TINYENGINE_OK;
}
