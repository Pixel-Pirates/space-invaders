/*
 * bulletCollid.c
 *
 *  Created on: Feb 11, 2020
 *      Author: Raymond Bernardo
 */

#include "../bsp/bsp.h"

//fighter 100x85
const int FIGHTER_WIDTH = 100;
const int FIGHTER_HEIGHT = 85;

struct entinity
{
    int x;
    int y;
    int width;
    int height;
};

struct coord
{
    int x;
    int y;
};

bool bulletCollided(struct entinity entinity, struct entinity bullet)
{
    struct coord topLeft = {
         .x = entinity.x - entinity.width/2,
         .y = entinity.y + entinity.height/2
    };

    struct coord bottomRight = {
         .x = entinity.x - entinity.width/2,
         .y = entinity.y + entinity.height/2
    };

    if ((topLeft.x < bullet.x && bottomRight.x > bullet.x) &&
        (topLeft.y > bullet.y && bottomRight.y <bullet.y))
      return true;

    return false;
}


