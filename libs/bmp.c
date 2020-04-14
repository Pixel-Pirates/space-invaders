/*
 * bmp.c
 *
 *  Created on: Apr 14, 2020
 *      Author: Raymond Bernardo
 */

#include "../bsp/bsp.h"
#include "bmp.h"
extern SemaphoreHandle_t sd_ready;

uint16_t bmp_read_16(bmp_t* bmp, unsigned short* usBytesRead)
{
    uint16_t data[2];
    xSemaphoreTake(sd_ready, portMAX_DELAY);
    f_read(&bmp->file, data, 2, usBytesRead);
    xSemaphoreGive(sd_ready);
    return data[0] | (data[1] << 8);
}

void bmp_open(bmp_t* bmp, char* src)
{
    xSemaphoreTake(sd_ready, portMAX_DELAY);
    FRESULT fresult = f_open(&bmp->file, src, FA_READ);
    if(fresult != FR_OK) scia_msg("\rDid not open\n");

    bmp->startAddress = 0;

#ifdef BMP
    f_lseek(&bmp->file, 0xA);
    if(fresult != FR_OK) scia_msg("\rDid not seek\n");

    unsigned short usBytesRead;
    uint16_t data[4];
    fresult = f_read(&bmp->file, data, 4, &usBytesRead);

    uint32_t startAddress = data[0] | (data[1] << 8) | (data[2] << (uint32_t)16) | (data[3] << (uint32_t)24);

    fresult = f_lseek(&bmp->file, startAddress);
    if(fresult != FR_OK) scia_msg("\rDid not seek\n");
    bmp->startAddress = startAddress;
#endif

    xSemaphoreGive(sd_ready);
}

void bmp_read(bmp_t* bmp, char * data, int len, unsigned short* usBytesRead)
{
    xSemaphoreTake(sd_ready, portMAX_DELAY);
    f_read(&bmp->file, data, len, usBytesRead);
    xSemaphoreGive(sd_ready);
}

