#include <stdio.h>
#include "pico/stdlib.h"
#include "GameScene.h"
#include "tinyengine.h"
#include "Pong.h"

void PongGame::run() {
    TinyEngineFrameBuffer frame_buf(320, 240, true);
    TinyEngineRendererDVI dvi(frame_buf);
    TinyEngine engine(dvi);


    PongScene scene(frame_buf, dvi, engine);

    scene.create();

    engine.set_pre_inti_clbk([&]()
        {
            return TINYENGINE_OK;
        });
    engine.set_render_clbk([&]()
        {
            scene.render();
            return TINYENGINE_OK;
        });
    engine.set_update_clbk([&](double frameTime)
        {
            scene.update(frameTime);

            return TINYENGINE_OK;
        });

    engine.init();
    engine.start();
    engine.start_loop();
}

int main()
{
    set_frequency(); // We set this before stdio init
    stdio_init_all();

    PongGame pong;

    pong.run();
}
