/*
 * bulletCollid.h
 *
 *  Created on: Apr 9, 2020
 *      Author: Raymond Bernardo
 */

#ifndef LIBS_BULLETCOLLID_H_
#define LIBS_BULLETCOLLID_H_


typedef struct entity
{
    int x;
    int y;
    int width;
    int height;
} entity_t;

struct coord
{
    int x;
    int y;
};

bool bulletCollided(entity_t entity, entity_t bullet);


#endif /* LIBS_BULLETCOLLID_H_ */
