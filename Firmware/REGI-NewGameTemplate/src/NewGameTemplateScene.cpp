//
// Created by GRHRehabTech on 2025-08-07.
//

#include "NewGameTemplate.h"
#include "tinyengine.h"
#include "cute_c2.h"
#include "hardware/watchdog.h"
#include "pico/multicore.h"
#include "TinyEngineUI.h"


void NewGameTemplateScene::create() {
    GameScene::create();
}

void NewGameTemplateScene::render() {
    GameScene::render();
    m_framebuffer.clear(0);

    m_framebuffer.draw_string("New Game Template", 50, 50, 15);

    m_renderer.wait_for_vsync();
    m_framebuffer.swap_blocking();
}


void NewGameTemplateScene::update(double frameTime) {
    GameScene::update(frameTime);
}

void NewGameTemplateScene::destroy() {
    GameScene::destroy();
}
