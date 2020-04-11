/*
 * bullet.c
 *
 *  Created on: Apr 8, 2020
 *      Author: Raymond Bernardo
 */

#include "../bsp/bsp.h"
#include "../bsp/device_driver/fatfs/src/tff.h"
#include "../game.h"
#include "../libs/bulletCollid.h"

#define BULLET_WIDTH 3
#define BULLET_HEIGHT 8

extern SemaphoreHandle_t bullet_ready;
extern SemaphoreHandle_t lcd_ready;
extern player_t player;
extern invader_t invaders[27];
void draw_entinity(entinity_t entinity, uint16_t color);
extern bool gameOver;
extern bool playerShootSound;

void bulletTask()
{
    while(1)
    {
        while(gameOver);
        xSemaphoreTake(bullet_ready, portMAX_DELAY);
        playerShootSound = true;
        entinity_t bullet;
        bullet.width = BULLET_WIDTH;
        bullet.height = BULLET_HEIGHT;
        bullet.x = player.sprite.x + player.sprite.width/2;
        bullet.y = player.sprite.y - player.sprite.height;

        while(1)
        {
            draw_entinity(bullet, 0x0000);

            if (bullet.y <= 0)
            {
                break;
            }

            int foundDead = false;
            entinity_t invader_e;
            for (int i = 0; i < INVADER_COLUMNS*INVADER_ROWS; i++)
            {
                invader_t invader = invaders[i];
                if (!invader.alive)
                {
                    continue;
                }
                invader_e.width = invader.sprite.width;
                invader_e.height = invader.sprite.height;
                invader_e.x = invader.sprite._x;
                invader_e.y = invader.sprite._y;

                if (bulletCollided(invader_e, bullet)) {
                    invaders[i].alive = false;
                    foundDead = true;
                    draw_entinity(invader_e, 0x0000);
                    break;
                }
            }

            if (foundDead) {
                break;
            }

            bullet.y -= 1;
            draw_entinity(bullet, 0xFFE0);
            vTaskDelay(5 / portTICK_PERIOD_MS);
        }
    }
}

void draw_entinity(entinity_t entinity, uint16_t color) {
    xSemaphoreTake(lcd_ready, portMAX_DELAY);
    LCD_DrawRectangle(entinity.x, entinity.x + entinity.width, entinity.y, entinity.y + entinity.width, color);
    xSemaphoreGive(lcd_ready);
}
