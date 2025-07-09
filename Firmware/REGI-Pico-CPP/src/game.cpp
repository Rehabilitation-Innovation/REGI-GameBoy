#include <stdio.h>
#include <math.h>

#include "tinyengine.h"
#include "tinyengine_renderer.h"
#include "tinyengine_framebuffer.h"
#include "tinyengine_sprite.h"
#include "tinyengine_sprite_animation.h"
#include "tinyengine_audio.h"

#include "tinyengine_renderer_dvi_c_adapter.h"

#include "pico/stdlib.h"
#include "pico/rand.h"
#include "hardware/pwm.h"
#include "memory.h"
#include <malloc.h>
#include <math.h>

#include "game.h"
#include "game_scene.h"

void Game::run() {
    TinyEngineFrameBuffer frame_buf(320, 240, true);
    TinyEngineRendererDVI dvi(frame_buf);
    TinyEngine engine(dvi);

    GameScene scene(frame_buf, dvi);
    engine.set_pre_inti_clbk([&]() {
        telog("Hello From Callback");
        return TINYENGINE_OK;
        });
    engine.set_render_clbk([&]() {
        scene.render();
        return TINYENGINE_OK;
        });
    engine.set_update_clbk([&](double frameTime) {
        scene.update(frameTime);

        return TINYENGINE_OK;
        });
    engine.init();

    engine.start();
    engine.start_loop();

}