//
// Created by GRHRehabTech on 2025-08-07.
//

#ifndef PONG_H
#define PONG_H
#include "Game.h"
#include "GameScene.h"

class PongGame : public Game {
public:
    void run() override;
};

class PongScene : public GameScene {
public:
    ~PongScene() override;

    PongScene(TinyEngineFrameBuffer& _framebuffer, TinyEngineRendererI& _renderer, TinyEngine& _engine)
        : GameScene(_framebuffer, _renderer, _engine) {
    }

    void create() override;

    void render() override;

    void update(double frameTime) override;

    void destroy() override;
};



#endif //PONG_H
