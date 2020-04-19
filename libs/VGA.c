/*
 * VGA.c
 *
 *  Created on: Apr 14, 2020
 *      Author: Tomek
 */

#include "libs/VGA.h"
#include "bsp/bsp.h"
#include "bsp/device_driver/fatfs/src/tff.h"
#include "libs/sprite.h"
#include "threads/thread.h"

extern SemaphoreHandle_t sd_ready;
extern player_t player;

inline void writeAll(uint16_t color)
{
    uint32_t addr = 0;

    sram_write_multi_start();

    for(uint32_t x = 0; x < 640; x++)
    {
        for(uint32_t y = 0; y < 480; y++)
        {
            addr = (x << 9) | y;
            sram_write_multi(addr, color);
        }
    }

    sram_write_multi_end();
}

void clearScreen()
{
    uint32_t addr = 0;
    for(unsigned sramNum = 0; sramNum < 2; sramNum++)
    {
        GPIO_WritePin(32, sramNum);
        sram_write_multi_start();
        for(uint32_t x = 0; x < 320; x++)
        {
            for(uint32_t y = 0; y < 240; y++)
            {
                addr = ((VGA_OFFSET_X + x) << 9) | (VGA_OFFSET_Y + y);
                sram_write_multi(addr, 0);
            }
        }
        sram_write_multi_end();
    }
}

void loadPlayer()
{
    GPIO_WritePin(32, 0);
    sprite_draw(&player.sprite);

    GPIO_WritePin(32, 1);
    sprite_draw(&player.sprite);

    GPIO_WritePin(32, 0);
}

void drawBackGround()
{
    drawImage("back.txt", 640, 480, 0, 0);
}

void drawImage(char* fileName, uint32_t width, uint32_t height, uint32_t x_offset, uint32_t y_offset)
{
    FRESULT result;
    FIL fileObj;
    unsigned short usBytesRead;

    xSemaphoreTake(sd_ready, portMAX_DELAY);
    result = f_open(&fileObj, fileName, FA_READ);

    uint32_t addr = 0;
    uint16_t buf[2] = {0, 0};

    for(unsigned sramNum = 0; sramNum < 2; sramNum++)
    {
        GPIO_WritePin(32, sramNum);
        sram_write_multi_start();
        for(uint32_t x = 0; x < width; x++)
        {
            for(uint32_t y = 0; y < height; y++)
            {
                addr = ((x_offset + x) << 9) | (y_offset + y);
                result = f_read(&fileObj, buf, 2, &usBytesRead);
                sram_write_multi(addr, buf[1] << 8 | buf[0]);
            }
        }
        sram_write_multi_end();
        result = f_lseek(&fileObj, 0);
    }

    result = f_close(&fileObj);
    xSemaphoreGive(sd_ready);
    GPIO_WritePin(32, 0);
}

void win()
{
    drawImage("W.txt", 206, 60, VGA_OFFSET_X + 57, VGA_OFFSET_Y + 90);
}

void loss()
{
    drawImage("L.txt", 206, 60, VGA_OFFSET_X + 57, VGA_OFFSET_Y + 90);
}
