#include <stdio.h>
#include <math.h>

#include "tinyengine.h"
#include "tinyengine_renderer.h"
#include "tinyengine_framebuffer.h"
#include "tinyengine_sprite.h"

#include "tinyengine_audio.h"

#include "tinyengine_renderer_dvi.h"

#include "pico/stdlib.h"
#include "pico/rand.h"
#include "hardware/pwm.h"
#include "memory.h"
#include <malloc.h>
#include <math.h>

#include "Game.h"

// #include "BootLoaderScreen.h"
// #include "DinoGame.h"
#include "GameScene.h"
// #include "Pong.h"
// #include "scenes/SnakeGame.h"

// void Game::run()
// {
//     TinyEngineFrameBuffer frame_buf(320, 240, true);
//     TinyEngineRendererDVI dvi(frame_buf);
//     TinyEngine engine(dvi);

//     // SnakeGame scene(frame_buf, dvi, engine);
//     // DinoGame scene(frame_buf, dvi, engine);
//     Pong scene(frame_buf, dvi, engine);
//     // BootLoaderScreen scene(frame_buf, dvi, engine);
//     scene.create();

//     engine.set_pre_inti_clbk([&]()
//         {
//             return TINYENGINE_OK;
//         });
//     engine.set_render_clbk([&]()
//         {
//             scene.render();
//             return TINYENGINE_OK;
//         });
//     engine.set_update_clbk([&](double frameTime)
//         {
//             scene.update(frameTime);

//             return TINYENGINE_OK;
//         });

//     engine.init();
//     engine.start();
//     engine.start_loop();
// }
