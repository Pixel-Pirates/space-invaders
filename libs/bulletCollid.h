/*
 * bulletCollid.h
 *
 *  Created on: Apr 9, 2020
 *      Author: Raymond Bernardo
 */

#ifndef LIBS_BULLETCOLLID_H_
#define LIBS_BULLETCOLLID_H_


typedef struct entinity
{
    int x;
    int y;
    int width;
    int height;
} entinity_t;

struct coord
{
    int x;
    int y;
};

bool bulletCollided(entinity_t entinity, entinity_t bullet);


#endif /* LIBS_BULLETCOLLID_H_ */
