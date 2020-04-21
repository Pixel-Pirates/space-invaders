/*
 * VGA.h
 *
 *  Created on: Apr 14, 2020
 *      Author: Tomek
 */

#ifndef LIBS_VGA_H_
#define LIBS_VGA_H_

#include <stdint.h>

#define VGA_WHITE   0xFFF
#define VGA_RED     0XF00
#define VGA_GREEN   0X0F0
#define VGA_BLUE    0X00F
#define VGA_BLACK   0X000

inline void writeAll(uint16_t color);
void clearScreen();
void drawBackGround();
void loadPlayer();
void drawImage(char* fileName, uint32_t width, uint32_t height, uint32_t x_offset, uint32_t y_offset);
void win();
void loss();
void VGA_Text(uint16_t X_pos, uint16_t Y_pos, unsigned char *str, uint16_t Color);

#endif /* LIBS_VGA_H_ */
