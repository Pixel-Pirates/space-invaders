/*
 * updateTask.c
 *
 *  Created on: Feb 11, 2020
 *      Author: Raymond Bernardo
 */

#include "../bsp/bsp.h"
#include "../libs/printNum.h"
#include "../bsp/device_driver/fatfs/src/tff.h"
#include "../game.h"
#include "../libs/sprite.h"
#include "../libs/bmp.h"
#include "../libs/bulletCollid.h"

#define JOY_DEADZONE 30
#define JOY_OFFSET 0

#pragma DATA_SECTION(playerPixels,"ramgs4")
#pragma DATA_SECTION(destruction1,"ramgs4")
#pragma DATA_SECTION(destruction2,"ramgs4")

char playerPixels[PLAYER_SIZE];
char destruction1[PLAYER_SIZE];
char destruction2[PLAYER_SIZE];

extern player_t player;
extern SemaphoreHandle_t player_ready;
extern SemaphoreHandle_t bullet_ready;
extern SemaphoreHandle_t lcd_ready;
extern volatile bool gameOver;
extern volatile bool victory;
extern volatile bool playerDead;
extern bool firstRun;
extern void draw_entity(entity_t entity, uint16_t color);

extern void setUpGame();

nunchuck_t getNunchuckData();

volatile uint16_t deadLoop;
volatile bool deadFrame;

uint16_t getData() {
   while(ScibRegs.SCIFFRX.bit.RXFFST == 0) { } // wait for empty state
   return ScibRegs.SCIRXBUF.all & 0xFF;
}

void updateTask(void * pvParameters)
{
    int cPressed = false;
    bmp_t player_bmp;
    bmp_t sd1_bmp;
    bmp_t sd2_bmp;
    unsigned short usBytesRead;
    int16_t RESTING_X = 127;
#ifndef WIRELESS_CONTROLLER
    RESTING_X = getNunchuckData().joy_x;
#endif

#ifdef BMP
    #ifdef SMALL_SPRITES
        bmp_open(&player_bmp, "s.bmp");
    #else
    bmp_open(&player_bmp, "shooter.bmp");
    #endif
#elif RAW
    #ifdef SMALL_SPRITES
        bmp_open(&player_bmp, "s.txt");
        bmp_open(&sd1_bmp, "sd1.txt");
        bmp_open(&sd2_bmp, "sd2.txt");
    #else
        bmp_open(&player_bmp, "shooter.txt");
    #endif
#endif

    bmp_read(&player_bmp, playerPixels, PLAYER_SIZE, &usBytesRead);
    bmp_read(&sd1_bmp, destruction1, PLAYER_SIZE, &usBytesRead);
    bmp_read(&sd2_bmp, destruction2, PLAYER_SIZE, &usBytesRead);

    player.sprite.data = playerPixels;
    sprite_draw(&player.sprite);

    while (1)
    {
    #ifdef WIRELESS_CONTROLLER
        //start condition is 0x37
        while (getData() != 0x37);
        uint16_t cbutton = getData();
        if (cbutton == 0x44) { //check for escape
            continue;
        }
        uint16_t zbutton = getData();
        uint16_t xpos = getData();
        if (xpos == 0x44) { //check for escape again
            xpos = getData();
        }

        nunchuck_t nunchuck;
        nunchuck.button_c = cbutton;
        nunchuck.button_z = zbutton;
        nunchuck.joy_x = xpos;
    #else
        nunchuck_t nunchuck = getNunchuckData();
    #endif

        if(((gameOver && !victory) || playerDead) && !firstRun )
        {
            if((deadLoop >> 5) & 1)
            {
                player.sprite._x = 0;               /* Used to FORCE a redraw */
                if(deadFrame)
                    player.sprite.data = destruction1;
                else
                    player.sprite.data = destruction2;
                deadFrame = !deadFrame;
                sprite_draw(&player.sprite);
                deadLoop = 0;
            }

            deadLoop++;

        }

        if (gameOver) {
            if (!nunchuck.button_z) {
                firstRun = false;
                setUpGame();
                player.sprite._x = 0;               /* Used to FORCE a redraw */
                player.sprite.data = playerPixels;
                sprite_draw(&player.sprite);
                gameOver = false;
                playerDead = false;
                deadLoop = 0;
            }
            continue;
        }
        else if(playerDead){
            if (!nunchuck.button_z)
            {
                /* Undraw old location */
                entity_t player_e;
                player_e.height = player.sprite.height;
                player_e.width = player.sprite.width;
                player_e.x = player.sprite.x;
                player_e.y = player.sprite.y;
                draw_entity(player_e, 0x0000);

                /* Player Respawn */
                player.sprite.data = playerPixels;
                player.sprite.x = PLAYER_START_X;   /* Re-snap to start */
                player.sprite._x = 0;               /* Used to FORCE a redraw */
                sprite_draw(&player.sprite);        /* Redraw player, completing respawn */
                deadLoop = 0;
                playerDead = false;
            }
            continue;
        }

        int16_t delta = nunchuck.joy_x - RESTING_X;
        int16_t change = 0;

        if (delta > JOY_DEADZONE+JOY_OFFSET) {
            change = 3;
        }
        else if (delta < -JOY_DEADZONE+JOY_OFFSET) {
            change = -3;
        }

        if (change != 0) {
            xSemaphoreTake(player_ready, portMAX_DELAY);
            player.sprite.x += change;
            xSemaphoreGive(player_ready);
            //check if passed screen
            if (player.sprite.x < MIN_SCREEN_X || (player.sprite.x + player.sprite.width) > MAX_SCREEN_X) {
                player.sprite.x -= change;
            }
            else {
                sprite_draw(&player.sprite);
            }
        }

        if (!nunchuck.button_c) {
            if (!cPressed) {
                xSemaphoreGive(bullet_ready);
            }
            cPressed = true;
        }
        else if (cPressed)
        {
            cPressed = false;
        }
    }
}

nunchuck_t getNunchuckData() {
    nunchuck_refresh();
    vTaskDelay(5 / portTICK_PERIOD_MS);
    nunchuck_send_read();
    vTaskDelay(5 / portTICK_PERIOD_MS);
    return nunchuck_read();
}

