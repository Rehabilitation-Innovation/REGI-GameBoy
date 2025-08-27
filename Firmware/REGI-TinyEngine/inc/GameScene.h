/**
 *@file GameScene.h
 * @author A. Singh (--)
 * @brief Abstract class for the actual game scene and its assets.
 * @version 1.0
 * @date 2025-08-26
 *
 * Glenrose Rehabilitation Research and Innovation (C) 2025
 *
 */

#pragma once
#include "tinyengine_scene.h"
#include "tinyengine_sprite.h"
#include "tinyengine_framebuffer.h"
#include "tinyengine_renderer_dvi.h"
#include <vector>

 /**
  *@brief Contains all of the common fucctions and members needed to render a scene. A down stream Game scene will use the frame buffer and engine refrences stored here to render.
  *
 */
class GameScene : public SceneI {
protected: // debatable
    /**
     * @brief Local refrance for framebuffer. User calls this instance to render.
     * 
     */
    TinyEngineFrameBuffer& m_framebuffer;
    /**
     *@brief Local interface for renderer. This can be any renderer but only used for vsync.
     * 
     */
    TinyEngineRendererI& m_renderer;
    /**
     *@brief Local Engine refrence. Used for assigning fps limit, profiling and binding GPIOs.
     * 
     */
    TinyEngine& m_engine;
    /**
     *@brief Unused. But this is how a global ECS system would store sprites.
     * 
     */
    std::vector<Sprite*> sprites;

public:
    /**
     *@brief Construct a new Game Scene object
     *
     * @param _framebuffer
     * @param _renderer
     * @param _engine
     */
    GameScene(TinyEngineFrameBuffer& _framebuffer, TinyEngineRendererI& _renderer, TinyEngine& _engine)
        : m_framebuffer(_framebuffer), m_renderer(_renderer), m_engine(_engine) {
    };

    /**
     *@brief Destroy the Game Scene object
     *
     */
    ~GameScene();

    /**
     *@brief Create the scene after the object is created. This is for having multiple refrences to the same scene. Memory and configuration should only happen in this function.
     *
     */
    void create() override;

    /**
     *@brief All framebuffer operations are done here and this is called only once per frame. 
     *
     */
    void render() override;

    /**
     *@brief Game updates and other time dependednt modifications should be done here, there is no gurentee this is run only 
     * 
     * @param frameTime 
     */
    void update(double frameTime) override;

    /**
     *@brief Destroy scene here. Only call if you do not need to use scene again. Ideally this would be stored in PSRAM instead.
     * 
     */
    void destroy() override;
};
