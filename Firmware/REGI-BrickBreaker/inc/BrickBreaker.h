//
// Created by GRHRehabTech on 2025-08-07.
//
#pragma once
#include "Game.h"
#include "GameScene.h"

class BrickBreakerGame : public Game {
public:
    void run() override;
};

class BrickBreakerScene : public GameScene {
public:
    ~BrickBreakerScene() override;

    BrickBreakerScene(TinyEngineFrameBuffer& _framebuffer, TinyEngineRendererI& _renderer, TinyEngine& _engine)
        : GameScene(_framebuffer, _renderer, _engine) {
    }

    void create() override;

    void render() override;

    void update(double frameTime) override;

    void destroy() override;
};
