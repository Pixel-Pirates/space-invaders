/*
 * lcd.c
 *
 *  Created on: Apr 4, 2020
 *      Author: Raymond Bernardo
 */

#include "../bsp/bsp.h"
#include "../libs/printNum.h"
#include "../bsp/device_driver/fatfs/src/tff.h"

static FATFS g_sFatFs;
static FIL g_sFileObject;

void lcd() {

//    scia_msg("\rOpen SD Card\n");
//    FRESULT fresult = f_mount(0, &g_sFatFs);
//    if(fresult != FR_OK) scia_msg("\rDid not mount\n");
//
//
//    fresult = f_open(&g_sFileObject, "blue.bmp", FA_READ);
//    if(fresult != FR_OK) scia_msg("\rDid not open\n");
//
//    f_lseek(&g_sFileObject, 0xA);
//    if(fresult != FR_OK) scia_msg("\rDid not seek\n");
//
//    unsigned short usBytesRead;
//    uint16_t data[4];
//    fresult = f_read(&g_sFileObject, data, 4, &usBytesRead);
//
//    uint32_t startAddress = data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
//
//    f_lseek(&g_sFileObject, startAddress);
//    if(fresult != FR_OK) scia_msg("\rDid not seek\n");

    while (1) {
//        uint16_t data[4];
//        fresult = f_read(&g_sFileObject, data, 2, &usBytesRead);
//
//
//        LCD_Text(1,1,"AA",0xfefe);
        LCD_DrawRectangle(100, 110, 0, 10, 0x3333);
    }

}

