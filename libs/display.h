/*
 * display.h
 *
 *  Created on: Apr 20, 2020
 *      Author: Tomek
 */

#ifndef LIBS_DISPLAY_H_
#define LIBS_DISPLAY_H_

#include <stdint.h>

void text(uint16_t X_pos, uint16_t Y_pos, unsigned char *str, uint16_t Color);
void printScore();
void printLives();

#endif /* LIBS_DISPLAY_H_ */
