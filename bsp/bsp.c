#include "bsp.h"
#include "device_driver/fatfs/src/tff.h"

static FATFS g_sFatFs;

void BSP_Init()
{
    uart_init();
    LCD_Init(false);
    I2CA_Init();
    nunchuck_init();
    sram_init();

    scia_msg("\rOpen SD Card\n");
    FRESULT fresult = f_mount(0, &g_sFatFs);
    if(fresult != FR_OK) scia_msg("\rDid not mount\n");

}
