#include "bsp.h"



void BSP_Init()
{
    uart_init();
    LCD_Init(false);
    //I2CA_Init();
    //nunchuck_init();
    sram_init();
}
