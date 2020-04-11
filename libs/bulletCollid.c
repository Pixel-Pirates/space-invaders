/*
 * bulletCollid.c
 *
 *  Created on: Feb 11, 2020
 *      Author: Raymond Bernardo
 */

#include "../bsp/bsp.h"
#include "./bulletCollid.h"

//fighter 100x85
const int FIGHTER_WIDTH = 100;
const int FIGHTER_HEIGHT = 85;

bool bulletCollided(entinity_t entinity, entinity_t bullet)
{
    struct coord topLeft = {
         .x = entinity.x,
         .y = entinity.y
    };

    struct coord bottomRight = {
         .x = entinity.x + entinity.width,
         .y = entinity.y + entinity.height
    };

    bullet.x += bullet.width;
    bullet.y += bullet.height;

    if ((topLeft.x < bullet.x && bottomRight.x > bullet.x) &&
        (topLeft.y < bullet.y && bottomRight.y > bullet.y))
      return true;

    return false;
}


