///*
// * lcd.c
// *
// *  Created on: Apr 4, 2020
// *      Author: Raymond Bernardo
// */
//
#include "../bsp/bsp.h"
#include "../libs/printNum.h"
#include "../libs/sprite.h"
#include "../libs/bmp.h"
#include "../libs/VGA.h"

extern SemaphoreHandle_t player_ready;
extern invader_t invaders[27];
extern player_t player;
extern volatile bool gameOver;

#pragma DATA_SECTION(invaderM1Pixels,"ramgs4")
#pragma DATA_SECTION(invaderM2Pixels,"ramgs5")

#define INVADER_BASE_SPEED 50

char invaderM1Pixels[INVADER_SIZE];
char invaderM2Pixels[INVADER_SIZE];

void invaderTask() {

    unsigned short usBytesRead;
    bmp_t invader_med1;
    bmp_t invader_med2;

#ifdef BMP
    bmp_open(&invader_med1, "invaderA.bmp");
    bmp_open(&invader_med2, "invaderB.bmp");
#elif RAW
    bmp_open(&invader_med1, "invaderA.txt");
    bmp_open(&invader_med2, "invaderB.txt");
#endif

    bmp_read(&invader_med1, invaderM1Pixels, INVADER_SIZE, &usBytesRead);
    bmp_read(&invader_med2, invaderM2Pixels, INVADER_SIZE, &usBytesRead);
    int movingRight = true;
    int hitEnd = false;
    bool aActive = true;
    int count = 0;

    while (1) {
        while(gameOver);
        int invadersDead = 0;
        for (int i = 0; i < INVADER_COLUMNS*INVADER_ROWS; i++)
        {
            invader_t* invader = &invaders[i];
            if (!invader->alive) {
                invadersDead++;
                continue;
            }

            if (aActive) {
                invader->sprite.data = invaderM1Pixels;
            }
            else {
                invader->sprite.data = invaderM2Pixels;
            }
            sprite_draw(&invader->sprite);

            if (movingRight) {
                invader->sprite.x += 1;
                if (invader->sprite.x + invader->sprite.width > MAX_SCREEN_X - 1)
                {
                    hitEnd = true;
                }
            } else {
                invader->sprite.x -= 1;

                if (invader->sprite.x <= 0)
                {
                    hitEnd = true;
                }
            }

        }

        if (hitEnd) {
            hitEnd = false;
            movingRight = !movingRight;
            for (int count = 0; count < 5; count++)
            {
                for (int i = 0; i < INVADER_COLUMNS*INVADER_ROWS; i++)
                {
                    invader_t* invader = &invaders[i];
                    if (!invader->alive) continue;
                    invader->sprite.y += 1;
                    sprite_draw(&invader->sprite);
                }
            }
        }

        //check to see if image should be updated
        if (count++ > 10){
            aActive = !aActive;
            count = 0;
        }

        if (invadersDead == INVADER_ROWS*INVADER_COLUMNS) {
            gameOver = true;
            #ifdef VGA
                win();
            #endif
        }

        vTaskDelay(INVADER_BASE_SPEED / portTICK_PERIOD_MS);
    }
}
