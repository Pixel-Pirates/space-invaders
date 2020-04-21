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

#define JOY_DEADZONE 30
#define JOY_OFFSET 0

#pragma DATA_SECTION(playerPixels,"ramgs4")
char playerPixels[PLAYER_SIZE];

extern player_t player;
extern SemaphoreHandle_t player_ready;
extern SemaphoreHandle_t bullet_ready;
extern SemaphoreHandle_t lcd_ready;
extern volatile bool gameOver;
extern void setUpGame();

nunchuck_t getNunchuckData();

uint16_t getData() {
   while(ScibRegs.SCIFFRX.bit.RXFFST == 0) { } // wait for empty state
   return ScibRegs.SCIRXBUF.all & 0xFF;
}

void updateTask(void * pvParameters)
{
    int cPressed = false;
    bmp_t player_bmp;
    unsigned short usBytesRead;
    int16_t RESTING_X = getNunchuckData().joy_x;

#ifdef BMP
    bmp_open(&player_bmp, "shooter.bmp");
#elif RAW
    bmp_open(&player_bmp, "shooter.txt");
#endif

    bmp_read(&player_bmp, playerPixels, PLAYER_SIZE, &usBytesRead);
    player.sprite.data = playerPixels;
    sprite_draw(&player.sprite);


    while (1)
    {
        //start condition is 0x37
//        while (getData() != 0x37);
//        uint16_t cbutton = getData();
//        if (cbutton == 0x44) { //check for escape
//            continue;
//        }
//        uint16_t zbutton = getData();
//        uint16_t xpos = getData();
//        if (xpos == 0x44) {
//            xpos = getData();
//        }
//
//        nunchuck_t nunchuck;
//        nunchuck.button_c = cbutton;
//        nunchuck.button_z = zbutton;
//        nunchuck.joy_x = xpos;

        nunchuck_t nunchuck = getNunchuckData();

        if (gameOver) {

            if (!nunchuck.button_z) {
                setUpGame();

                sprite_draw(&player.sprite);

                gameOver = false;
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

