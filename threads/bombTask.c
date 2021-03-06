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
#include "../libs/VGA.h"
#include "../libs/sprite.h"
#include "../libs/display.h"

extern invader_t invaders[27];
extern player_t player;

void draw_entity(entity_t entity, uint16_t color);

extern volatile bool gameOver;
extern volatile bool playerDead;
extern volatile bool playPlayerDeadSound;
extern volatile bool victory;

extern volatile uint16_t deadLoop;

extern char * destruction1;

void bombTask(){
    bool activeBomb = false;
    entity_t bomb;
    bomb.x = 0;
    bomb.y = 0;
    bomb.height = 5;
    bomb.width = 4;
    while(1) {
        while(gameOver || playerDead);

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

            bomb.x = bomber.sprite.x + bomber.sprite.width/2;
            bomb.y = bomber.sprite.y + bomber.sprite.height;

            activeBomb = true;
        }

        bomb.y++;
//        draw_entity(bullet, 0xFFE0);
        draw_entity(bomb, 0xFFF);
        vTaskDelay(5 / portTICK_PERIOD_MS);
        draw_entity(bomb, 0x0000);

        entity_t player_e;
        player_e.height = player.sprite.height;
        player_e.width = player.sprite.width;
        player_e.x = player.sprite.x;
        player_e.y = player.sprite.y;

        if (bulletCollided(player_e, bomb)) {
            activeBomb = false;

            player.lives--;
            printLives();
            playPlayerDeadSound = true;

            if(player.lives == 0)
            {
                gameOver = true;
                victory = false;
                #ifdef VGA
                    loss();
                #endif
            }
            else
            {
                player.sprite._x = 0;               /* Used to FORCE a redraw */
                deadLoop = 0xFFFF;
                playerDead = true;                  /* Set flag */
            }
        }

        if (bomb.y + bomb.height  >= MAX_SCREEN_Y - 1 - (SPRITE_OFFSET_Y>>2)) {
            bomb.y = 0;
            activeBomb = false;
        }

    }
}

