#include "bsp.h"



void BSP_Init()
{
    uart_init();
    I2CA_Init();
    nunchuck_init();
}
