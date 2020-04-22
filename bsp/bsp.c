#include "bsp.h"
#include "device_driver/fatfs/src/tff.h"

static FATFS g_sFatFs;

void BSP_Init()
{
    uart_init();

#ifdef LCD
    LCD_Init(false);
#endif

#ifndef WIRELESS_CONTROLLER
    I2CA_Init();
    nunchuck_init();
#endif
    sram_init();

    scia_msg("\rOpen SD Card\n");
    FRESULT fresult = f_mount(0, &g_sFatFs);
    if(fresult != FR_OK) scia_msg("\rDid not mount\n");

}
