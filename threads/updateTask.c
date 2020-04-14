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

#define DEADZONE 40


extern player_t player;
extern SemaphoreHandle_t player_ready;
extern SemaphoreHandle_t bullet_ready;
extern SemaphoreHandle_t lcd_ready;
extern volatile bool gameOver;
extern void setUpGame();

nunchuck_t getNunchuckData();

void updateTask(void * pvParameters)
{
    int cPressed = false;
    sprite_draw(&player.sprite);
    int16_t RESTING_X = getNunchuckData().joy_x;

    while (1)
    {
        nunchuck_t nunchuck = getNunchuckData();

        if (gameOver) {

            if (!nunchuck.button_z) {
                setUpGame();

                xSemaphoreTake(lcd_ready, portMAX_DELAY);
                LCD_Init(false);
                xSemaphoreGive(lcd_ready);
                sprite_draw(&player.sprite);

                gameOver = false;
            }

            continue;
        }

        int16_t delta = nunchuck.joy_x - RESTING_X;
        int16_t change = 0;



        if (delta > 0) {
            change = 3;
        }
        else if (delta < 0) {
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

