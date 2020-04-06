/*
 * updateTask.c
 *
 *  Created on: Feb 11, 2020
 *      Author: Raymond Bernardo
 */

#include "../bsp/bsp.h"
#include "../libs/printNum.h"

void updateTask(void * pvParameters)
{
    //int waiting = false;
    while (1)
    {
        scia_msg("\rupdate\n");
        nunchuck_refresh();
        vTaskDelay(100 / portTICK_PERIOD_MS);
        nunchuck_send_read();
        vTaskDelay(100 / portTICK_PERIOD_MS);
        nunchuck_t data = nunchuck_read();
        nunchuck_print(&data);




//        if (waiting)
//        {
//            if (xSemaphoreTake( wii_ready, 1000 ) == pdTRUE)
//            {
//                uint8_t data[6];
//                int i;
//                for(i=0; i < 6; i++)
//                    data[i] = (I2caRegs.I2CDRR.all ^ 0x17) + 0x17;
//
//
//                scia_msg((data[0] & 0x01) + 40);
//
//                waiting = false;
//            }
//        }
//        else
//        {
//            while (I2CA_WriteData(&I2cReq) != I2C_SUCCESS);
//            vTaskDelay(500 / portTICK_PERIOD_MS);
//            while (I2CA_ReadData(&I2cMsgIn1) != I2C_SUCCESS);
//            waiting = true;
//        }

    }
}


//
// i2c_int1a_isr - I2CA ISR
//
__interrupt void i2c_int1a_isr(void)
{
    Uint16 IntSource, i;

    //
    // Read __interrupt source
    //
    IntSource = I2caRegs.I2CISRC.all;

    //
    // Interrupt source = stop condition detected
    //
    if(IntSource == I2C_SCD_ISRC)
    {
//        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
//        xSemaphoreGiveFromISR( wii_ready, &xHigherPriorityTaskWoken );
//        portYIELD_FROM_ISR( xHigherPriorityTaskWoken );

        uint8_t data[6];
        int i;
        for(i=0; i < 6; i++)
            data[i] = (I2caRegs.I2CDRR.all ^ 0x17) + 0x17;
    }

    //
    // Interrupt source = Register Access Ready
    // This __interrupt is used to determine when the EEPROM address setup
    // portion of the read data communication is complete. Since no stop bit is
    // commanded, this flag tells us when the message has been sent instead of
    // the SCD flag. If a NACK is received, clear the NACK bit and command a
    // stop. Otherwise, move on to the read data portion of the communication.
    //
    else if(IntSource == I2C_ARDY_ISRC)
    {
        if(I2caRegs.I2CSTR.bit.NACK == 1)
        {
            I2caRegs.I2CMDR.bit.STP = 1;
            I2caRegs.I2CSTR.all = I2C_CLR_NACK_BIT;
        }
//        else if(CurrentMsgPtr->MsgStatus == I2C_MSGSTAT_SEND_NOSTOP_BUSY)
//        {
//            CurrentMsgPtr->MsgStatus = I2C_MSGSTAT_RESTART;
//        }
    }
    else
    {
        //
        // Generate some error due to invalid __interrupt source
        //
        __asm("   ESTOP0");
    }

    //
    // Enable future I2C (PIE Group 8) __interrupts
    //
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP8;
}
