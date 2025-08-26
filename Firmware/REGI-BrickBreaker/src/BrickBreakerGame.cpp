#include <stdio.h>
#include "pico/stdlib.h"
#include "GameScene.h"
#include "tinyengine.h"
#include "BrickBreaker.h"

void BrickBreakerGame::run() {
    TinyEngineFrameBuffer frame_buf(320, 240, true);
    TinyEngineRendererDVI dvi(frame_buf);
    TinyEngine engine(dvi);


    BrickBreakerScene scene(frame_buf, dvi, engine);

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

    BrickBreakerGame brick;

    brick.run();
    // uint32_t dist2 = 0;

    // gpio_init(40);
    // gpio_init(41);
    // gpio_set_dir(40, GPIO_OUT);
    // gpio_set_dir(41, GPIO_IN);
    // gpio_init(0);
    // gpio_set_dir(0, GPIO_OUT);

    // while (1) {
    //     gpio_put(0, 1);
    //     dist2 = getCm(40, 41);
    //     printf("%d \r\n", dist2);
    //     gpio_put(0, 0);
    //     sleep_ms(500);
    // }
}
