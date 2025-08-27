/**
 *@file tinyengine_scene.h
 * @author A. Singh (--)
 * @brief Pure virtual interface for scene dependency injection.
 * @version 1.0
 * @date 2025-08-27
 * 
 * Glenrose Rehabilitation Research and Innovation (C) 2025
 * 
 */
#pragma once

 /**
  *@brief Pure virtual interface for scene dependency injection. All scenes must implement this interface
  * This is seperated for convienience, a game scene can be seperate from a settings or player selection or score scene.
 */
class SceneI {
public:
    /**
     *@brief Destroy the Scene I object. Should be implemented as a seperate destroy function to allow for refrences.
     * 
     */
    virtual ~SceneI() = default;
    /**
     *@brief Called generically to create a scene.
     * 
     */
    virtual void create() = 0;
    /**
     *@brief Called generically to render a scene.
     * 
     */
    virtual void render() = 0;
    /**
     *@brief Called generically to update a scene.
     * 
     * @param frameTime passed time this frame.
     */
    virtual void update(double frameTime) = 0;
    /**
     *@brief Scenes should only be destryed by the user here.
     * 
     */
    virtual void destroy() = 0;
};