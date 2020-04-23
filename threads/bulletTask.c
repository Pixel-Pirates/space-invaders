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
#include "../libs/VGA.h"
#include "../libs/display.h"
#include "../libs/bulletCollid.h"

#define BULLET_WIDTH 3
#define BULLET_HEIGHT 8

extern SemaphoreHandle_t bullet_ready;
extern SemaphoreHandle_t lcd_ready;
extern invader_t invaders[27];
extern player_t player;
extern volatile bool gameOver;
extern volatile bool playerShootSound;
extern volatile bool invaderDiedSound;
extern uint16_t score;

void draw_entity(entity_t entity, uint16_t color);

void bulletTask()
{
    while(1)
    {
        xSemaphoreTake(bullet_ready, portMAX_DELAY);
    #ifdef SPEAKER
        playerShootSound = true;
    #endif
        entity_t bullet;
        bullet.width = BULLET_WIDTH;
        bullet.height = BULLET_HEIGHT;
        bullet.x = player.sprite.x + player.sprite.width/2;
        bullet.y = player.sprite.y - player.sprite.height;

        while(1)
        {
            draw_entity(bullet, 0x0000);
            if (bullet.y <= SPRITE_OFFSET_Y || gameOver)
            {
                break;
            }

            int foundDead = false;
            entity_t invader_e;
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
                    draw_entity(invader_e, 0x0000);
                    invaderDiedSound = true; //play invader death sound

                    /* Add Score */
                    score += invaders[i].type;
                    break;
                }
            }

            if (foundDead) {
                printScore();
                break;
            }

            bullet.y -= 1;
            draw_entity(bullet, 0xFFE0);
            vTaskDelay(5 / portTICK_PERIOD_MS);
        }
    }
}

void draw_entity(entity_t entity, uint16_t color) {
    xSemaphoreTake(lcd_ready, portMAX_DELAY);
    #ifdef VGA
    uint32_t addr = 0;

    for(uint16_t bufNum = 0; bufNum < 2; bufNum++)
    {
        GPIO_WritePin(32, bufNum);
        sram_write_multi_start();
        for(uint32_t y = 0; y < entity.height; y++)
        {
            for(uint32_t x = 0; x < entity.width; x++)
            {
                addr = ((entity.x + x + VGA_OFFSET_X) << 9) | (entity.y + y + VGA_OFFSET_Y);
                sram_write_multi(addr, color);
            }
        }
        sram_write_multi_end();
    }
    #elif LCD
    LCD_DrawRectangle(entity.x, entity.x + entity.width, entity.y, entity.y + entity.width, color);
    #endif
    xSemaphoreGive(lcd_ready);
}
