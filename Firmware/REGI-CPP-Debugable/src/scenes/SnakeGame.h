//
// Created by GRHRehabTech on 2025-07-15.
//

#pragma once
#include "game.h"
#include "GameScene.h"


class SnakeGame : public GameScene
{
public:
    SnakeGame(TinyEngineFrameBuffer& _framebuffer, TinyEngineRendererI& _renderer, TinyEngine& _engine)
        : GameScene(_framebuffer, _renderer, _engine)
    {
    }

    ~SnakeGame() override = default;

    void create() override;

    void render() override;

    void update(double frameTime) override;

    void destroy() override;
};
