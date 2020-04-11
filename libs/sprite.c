/*
 * sprite.c
 *
 *  Created on: Apr 8, 2020
 *      Author: Raymond Bernardo
 */
#include "../bsp/bsp.h"
#include "../bsp/device_driver/fatfs/src/tff.h"
#include "../game.h"

extern SemaphoreHandle_t lcd_ready;

void sprite_draw(sprite_t* sprite)
{
    if (sprite->_x == sprite->x && sprite->_y == sprite->y)
    {
        return;
    }

    xSemaphoreTake(lcd_ready, portMAX_DELAY);

    #ifdef VGA

    uint32_t addr = 0;
    sram_write_multi_start();
    for(uint32_t y = 0; y < sprite->height; y++)
    {
        for(uint32_t x = 0; x < sprite->width; x++)
        {
            addr = ((sprite->x + x) << 9) | (sprite->y + y);
            sram_write_multi(addr, sprite->data[x + y*sprite->width]);
        }
    }
    sram_write_multi_end();

    #elif LCD

    int len = sprite->height*sprite->width*2;
    LCD_WriteReg(HOR_ADDR_START_POS, sprite->y);     /* Horizontal GRAM Start Address */
    LCD_WriteReg(HOR_ADDR_END_POS, sprite->y + sprite->height - 1);  /* Horizontal GRAM End Address */
    LCD_WriteReg(VERT_ADDR_START_POS, sprite->x);    /* Vertical GRAM Start Address */
    LCD_WriteReg(VERT_ADDR_END_POS, sprite->x + sprite->width - 1);
    LCD_SetCursor(sprite->x, sprite->y);

    LCD_WriteIndex(GRAM);


    SPI_CS_LOW;
    LCD_Write_Data_Start();

    for (int i = 0; i < len - 1; i += 2)
    {
        uint16_t data16 = ((uint16_t)sprite->data[i + 1] << 8) | (uint16_t)sprite->data[i];
        LCD_Write_Data_Only(data16);
    }
    SPI_CS_HIGH;

    #endif


    sprite->_x = sprite->x;
    sprite->_y = sprite->y;
    xSemaphoreGive(lcd_ready);
}


