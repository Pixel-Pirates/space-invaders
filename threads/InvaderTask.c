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

#pragma DATA_SECTION(invaderS1Pixels,"ramgs5")
#pragma DATA_SECTION(invaderM1Pixels,"ramgs5")
#pragma DATA_SECTION(invaderL1Pixels,"ramgs5")

#pragma DATA_SECTION(invaderS2Pixels,"ramgs5")
#pragma DATA_SECTION(invaderM2Pixels,"ramgs5")
#pragma DATA_SECTION(invaderL2Pixels,"ramgs5")


char invaderS1Pixels[INVADER_SIZE];
char invaderM1Pixels[INVADER_SIZE];
char invaderL1Pixels[INVADER_SIZE];

char invaderS2Pixels[INVADER_SIZE];
char invaderM2Pixels[INVADER_SIZE];
char invaderL2Pixels[INVADER_SIZE];

void invaderTask() {

    unsigned short usBytesRead;
    bmp_t invader_sml1;
    bmp_t invader_sml2;
    bmp_t invader_med1;
    bmp_t invader_med2;
    bmp_t invader_lar1;
    bmp_t invader_lar2;
    int invaderSpeed = 50;

#ifdef BMP
    bmp_open(&invader_med1, "invaderA.bmp");
    bmp_open(&invader_med2, "invaderB.bmp");
#elif RAW
    #ifdef SMALL_SPRITES
        bmp_open(&invader_sml1, "is1.txt");
        bmp_open(&invader_med1, "im1.txt");
        bmp_open(&invader_lar1, "il1.txt");

        bmp_open(&invader_sml2, "is2.txt");
        bmp_open(&invader_med2, "im2.txt");
        bmp_open(&invader_lar2, "il2.txt");
    #else
        bmp_open(&invader_med1, "invaderA.txt");
        bmp_open(&invader_med2, "invaderB.txt");
    #endif
#endif
    bmp_read(&invader_sml1, invaderS1Pixels, INVADER_SIZE, &usBytesRead);
    bmp_read(&invader_med1, invaderM1Pixels, INVADER_SIZE, &usBytesRead);
    bmp_read(&invader_lar1, invaderL1Pixels, INVADER_SIZE, &usBytesRead);

    bmp_read(&invader_sml2, invaderS2Pixels, INVADER_SIZE, &usBytesRead);
    bmp_read(&invader_med2, invaderM2Pixels, INVADER_SIZE, &usBytesRead);
    bmp_read(&invader_lar2, invaderL2Pixels, INVADER_SIZE, &usBytesRead);

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
                if(invader->type == SMALL)
                    invader->sprite.data = invaderS1Pixels;
                else if(invader->type == MEDIUM)
                    invader->sprite.data = invaderM1Pixels;
                else
                    invader->sprite.data = invaderL1Pixels;
            }
            else {
                if(invader->type == SMALL)
                    invader->sprite.data = invaderS2Pixels;
                else if(invader->type == MEDIUM)
                    invader->sprite.data = invaderM2Pixels;
                else
                    invader->sprite.data = invaderL2Pixels;
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
        invaderSpeed = (INVADER_COLUMNS*INVADER_ROWS - invadersDead) + 1;

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

        vTaskDelay(invaderSpeed / portTICK_PERIOD_MS);
    }
}
