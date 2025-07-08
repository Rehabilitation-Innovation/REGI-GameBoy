#include "game_scene.h"
#include "tinyengine_framebuffer.h"

double x = 0, y = 0;



void GameScene::create() {};
void GameScene::destroy() {};
void GameScene::render() {

    m_framebuffer.clear(0x00);

    m_framebuffer.draw_filled_rectangle(x, y, 10, 10, 0xC0);

    // m_renderer.wait_for_vsync();
    m_framebuffer.swap_blocking();


};
void GameScene::update(double frameTime) {
    // telog("Update");
    x += 100 * frameTime;

    if (x >= 320) x = 0;

    y += 100 * frameTime;

    if (y >= 240) y = 0;

};


