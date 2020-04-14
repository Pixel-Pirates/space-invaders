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

bool bulletCollided(entity_t entity, entity_t bullet)
{
    struct coord topLeft = {
         .x = entity.x,
         .y = entity.y
    };

    struct coord bottomRight = {
         .x = entity.x + entity.width,
         .y = entity.y + entity.height
    };

    bullet.x += bullet.width;
    bullet.y += bullet.height;

    if ((topLeft.x < bullet.x && bottomRight.x > bullet.x) &&
        (topLeft.y < bullet.y && bottomRight.y > bullet.y))
      return true;

    return false;
}


