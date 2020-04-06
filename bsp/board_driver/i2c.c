/*
 * i2c.c
 *
 *  Created on: Feb 11, 2020
 *      Author: Raymond Bernardo
 */

#include "F28x_Project.h"     // Device Headerfile and Examples Include File

//
// I2CA_Init - Initialize I2CA settings
//
void I2CA_Init(void)
{
    GPIO_SetupPinMux(105, GPIO_MUX_CPU1, 1);
    GPIO_SetupPinMux(104, GPIO_MUX_CPU1, 1);

    GPIO_SetupPinOptions(105, GPIO_OPENDRAIN, GPIO_PULLUP);
    GPIO_SetupPinOptions(104, GPIO_OPENDRAIN, GPIO_PULLUP);

    I2caRegs.I2CSAR.all = 0x0050;     // Slave address - EEPROM control code

    I2caRegs.I2CPSC.all = 0xFF;         // Prescaler - need 7-12 Mhz on module clk
    I2caRegs.I2CCLKL = 10;            // NOTE: must be non zero
    I2caRegs.I2CCLKH = 5;             // NOTE: must be non zero
    I2caRegs.I2CIER.all = 0x24;       // Enable SCD & ARDY __interrupts

    I2caRegs.I2CMDR.all = (1 << 5); // Take I2C out of reset


    I2caRegs.I2CFFTX.all = 0x6000;    // Enable FIFO mode and TXFIFO
    I2caRegs.I2CFFRX.all = 0x2040;    // Enable RXFIFO, clear RXFFINT,

    return;
}

//
// I2CA_WriteData - Transmit I2CA message
//
Uint16 I2CA_WriteData(struct I2CMSG *msg)
{
    Uint16 i;

    //
    // Wait until the STP bit is cleared from any previous master communication.
    // Clearing of this bit by the module is delayed until after the SCD bit is
    // set. If this bit is not checked prior to initiating a new message, the
    // I2C could get confused.
    //
    if(I2caRegs.I2CMDR.bit.STP == 1)
    {
        return I2C_STP_NOT_READY_ERROR;
    }

    //
    // Check if bus busy
    //
    if(I2caRegs.I2CSTR.bit.BB == 1)
    {
        return I2C_BUS_BUSY_ERROR;
    }

    //
    // Setup slave address
    //
    I2caRegs.I2CSAR.all = msg->SlaveAddress;

    //
    // Setup number of bytes to send
    // MsgBuffer + Address
    //
    I2caRegs.I2CCNT = msg->NumOfBytes;

    //
    // Setup data to send
    //
    //I2caRegs.I2CDXR.all = msg->MemoryHighAddr;
    //I2caRegs.I2CDXR.all = msg->MemoryLowAddr;

    for (i=0; i < msg->NumOfBytes; i++)
    {
        I2caRegs.I2CDXR.all = *(msg->MsgBuffer+i);
    }

    //
    // Send start as master transmitter
    //
    I2caRegs.I2CMDR.all = 0x6E20;

    return I2C_SUCCESS;
}

//
// I2CA_ReadData - Read I2CA Message
//
Uint16 I2CA_ReadData(struct I2CMSG *msg)
{
    //
    // Wait until the STP bit is cleared from any previous master communication.
    // Clearing of this bit by the module is delayed until after the SCD bit is
    // set. If this bit is not checked prior to initiating a new message, the
    // I2C could get confused.
    //
    if(I2caRegs.I2CMDR.bit.STP == 1)
    {
        return I2C_STP_NOT_READY_ERROR;
    }
    //
    // Check if bus busy
    //
    if(I2caRegs.I2CSTR.bit.BB == 1)
    {
        return I2C_BUS_BUSY_ERROR;
    }

    I2caRegs.I2CSAR.all = msg->SlaveAddress;
    I2caRegs.I2CCNT = msg->NumOfBytes;    // Setup how many bytes to expect
    I2caRegs.I2CMDR.all = 0x2C20;         // Send restart as master receiver

    return I2C_SUCCESS;
}
