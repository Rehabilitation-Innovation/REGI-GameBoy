/**
 *@file Game.h
 * @author A. Singh (--)
 * @brief Abstract definition file for Games.
 * @version 1.0
 * @date 2025-08-26
 *
 * Glenrose Rehabilitation Research and Innovation (C) 2025
 *
 */

#pragma once
#include "main.h"

 /**
  *@brief Abstract definition file for games. This contains common functions all games need to implement.
 */
class Game {
public:
    /**
     *@brief Called by the entry point to run the game loop. Game must call startgame loop inside this function.
     *
     */
    virtual void run() {};
};


