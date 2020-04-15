/*
 * VGA.h
 *
 *  Created on: Apr 14, 2020
 *      Author: Tomek
 */

#ifndef LIBS_VGA_H_
#define LIBS_VGA_H_

#include <stdint.h>

inline void writeAll(uint16_t color);
void clearScreen();
void drawBackGround();
void loadPlayer();
void drawImage(char* fileName, uint32_t width, uint32_t height, uint32_t x_offset, uint32_t y_offset);
void win();
void loss();

#endif /* LIBS_VGA_H_ */
