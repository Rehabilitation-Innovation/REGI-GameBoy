//
// Created by GRHRehabTech on 2025-08-07.
//

#pragma once
#include "Game.h"
#include "GameScene.h"

class DinoGame : public Game {
public:
    void run() override;
};

class DinoScene : public GameScene {
public:
    ~DinoScene() override = default;

    DinoScene(TinyEngineFrameBuffer& _framebuffer, TinyEngineRendererI& _renderer, TinyEngine& _engine)
        : GameScene(_framebuffer, _renderer, _engine) {
    }

    void create() override;

    void render() override;

    void update(double frameTime) override;

    void destroy() override;
};

