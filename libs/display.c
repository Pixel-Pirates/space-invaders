/*
 * display.c
 *
 *  Created on: Apr 20, 2020
 *      Author: Tomek
 */

#include <libs/display.h>
#include "libs/VGA.h"
#include "bsp/bsp.h"
#include <stdint.h>
#include <stdio.h>
//#include "threads/thread.h"

extern SemaphoreHandle_t lcd_ready;
extern uint16_t score;
extern player_t player;

void text(uint16_t X_pos, uint16_t Y_pos, unsigned char *str, uint16_t Color)
{
    xSemaphoreTake(lcd_ready, portMAX_DELAY);

#ifdef LCD
    LCD_Text(X_pos, Y_pos, str, Color);
#elif VGA
    VGA_Text(X_pos, Y_pos, str, Color);
#endif

    xSemaphoreGive(lcd_ready);
}

void printScore()
{
    char scoreStr[4];
    sprintf(scoreStr, "%04i", score);

#ifdef LCD
    xSemaphoreTake(lcd_ready, portMAX_DELAY);
    LCD_DrawRectangle(HEADER_SCORE_NUM, HEADER_SCORE_NUM + 50, HEADER_Y, HEADER_Y + 20, 0x0000);
    xSemaphoreGive(lcd_ready);
#endif
   text(HEADER_SCORE_NUM, HEADER_Y, (unsigned char*) scoreStr, GREEN_COLOR);
}

void printLives()
{
    char livesStr[1];
    sprintf(livesStr, "%i", player.lives);

#ifdef LCD
    xSemaphoreTake(lcd_ready, portMAX_DELAY);
    LCD_DrawRectangle(HEADER_LIVES_NUM, HEADER_LIVES_NUM + 30, HEADER_Y, HEADER_Y + 20, 0x0000);
    xSemaphoreGive(lcd_ready);
#endif
    text(HEADER_LIVES_NUM, HEADER_Y, (unsigned char*) livesStr, GREEN_COLOR);
}
