/*
 * bomb.c
 *
 *  Created on: Apr 9, 2020
 *      Author: Raymond Bernardo
 */
#include <stdio.h>
#include <stdlib.h>
#include "../bsp/bsp.h"
#include "../bsp/device_driver/fatfs/src/tff.h"
#include "../game.h"
#include "../libs/bulletCollid.h"

extern invader_t invaders[27];
extern player_t player;
extern SemaphoreHandle_t lcd_ready;

void draw_entinity(entinity_t entinity, uint16_t color);

int gameOver = false;

void bombTask(){
    bool activeBomb = false;
    entinity_t bullet;
    bullet.x = 0;
    bullet.y = 0;
    bullet.height = 5;
    bullet.width = 4;
    while(1) {
        if (!activeBomb) {
            int invaderCount = 0;
            invader_t firstRow[INVADER_COLUMNS];
            for (int i = 1; i <= INVADER_COLUMNS; i++) {

                for (int j = INVADER_ROWS; j >= 1; j--) {

                      int invaderIndex = (INVADER_COLUMNS*j) - i;
                      invader_t invader = invaders[invaderIndex];

                      if (invader.alive) {

                            firstRow[invaderCount] = invader;
                            invaderCount++;
                            break;
                      }
                }
            }

            if (invaderCount == 0) {
                while (1); //GAME OVER
            }

            invader_t bomber = firstRow[rand() % invaderCount];

            bullet.x = bomber.sprite.x + bomber.sprite.width/2;
            bullet.y = bomber.sprite.y + bomber.sprite.height;

            activeBomb = true;
        }

        bullet.y++;
        draw_entinity(bullet, 0xFFE0);
        vTaskDelay(5 / portTICK_PERIOD_MS);
        draw_entinity(bullet, 0x0000);

        entinity_t player_e;
        player_e.height = player.sprite.height;
        player_e.width = player.sprite.width;
        player_e.x = player.sprite.x;
        player_e.y = player.sprite.y;

        //bool bulletCollided(entinity_t entinity, entinity_t bullet)
        if (bulletCollided(player_e, bullet)) {
            gameOver = true;
            while(gameOver);
        }

        if (bullet.y + bullet.height  >= MAX_SCREEN_Y - 1) {
            bullet.y = 0;
            activeBomb = false;
        }

    }
}
