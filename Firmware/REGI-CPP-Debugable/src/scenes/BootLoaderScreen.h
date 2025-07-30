//
// Created by GRHRehabTech on 2025-07-30.
//

#ifndef BOOTLOADERSCREEN_H
#define BOOTLOADERSCREEN_H
#include "GameScene.h"


class BootLoaderScreen : public GameScene {
public:
    ~BootLoaderScreen() override;

    void create() override;

    void render() override;

    void update(double frameTime) override;

    void destroy() override;

    BootLoaderScreen(TinyEngineFrameBuffer &_framebuffer, TinyEngineRendererI &_renderer, TinyEngine &_engine)
        : GameScene(_framebuffer, _renderer, _engine) {
    }
};


#endif //BOOTLOADERSCREEN_H
