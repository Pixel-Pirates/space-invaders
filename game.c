/*
 * game.c
 *
 *  Created on: Jan 17, 2020
 *      Author: Raymond Bernardo
 */
/* Kernel includes. */

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "FreeRTOS.h" /* Must come first. */
#include "task.h"     /* RTOS task related API prototypes. */
#include "queue.h"    /* RTOS queue related API prototypes. */
#include "timers.h"   /* Software timer related API prototypes. */
#include "semphr.h"   /* Semaphore related API prototypes. */

#include "game.h"
#include "bsp/board_driver/uart.h"
#include "stdbool.h"


#include "F28x_Project.h"     // Device Headerfile and Examples Include File


void   I2CA_Init(void);
Uint16 I2CA_WriteData(struct I2CMSG *msg);
Uint16 I2CA_ReadData(struct I2CMSG *msg);

//
// Defines
//
#define I2C_SLAVE_ADDR        0x52
#define I2C_NUMBYTES          2
#define I2C_EEPROM_HIGH_ADDR  0x00
#define I2C_EEPROM_LOW_ADDR   0x30

//
// Globals
//
struct I2CMSG I2cInit={ I2C_MSGSTAT_SEND_WITHSTOP,
                           I2C_SLAVE_ADDR, //Writing Address
                           2,
                           0xF0,                   // Msg Byte 1
                           0x55};

struct I2CMSG I2cReq={ I2C_MSGSTAT_SEND_WITHSTOP,
                           I2C_SLAVE_ADDR, //Writing Address
                           1,
                           0x00};

struct I2CMSG I2cMsgIn1={ I2C_MSGSTAT_SEND_NOSTOP,
                          I2C_SLAVE_ADDR,
                          6};

void printNum(int num)
{
    char buffer[100];
    int neg = 0;

    if (num < 0)
    {
        neg = 1;
        num *= -1;
    }
    int index = 0;
    if (num == 0)
    {
        buffer[index++] = 0x30;
    }


    while (num > 0)
    {
        buffer[index++] = (num % 10) + 0x30;
        num /= 10;
    }

    int size = index;
    buffer[size] = 0;

    for (int i = 0; i < size/2; i++)
    {
        char temp = buffer[size - (i + 1)];
        buffer[size - (i + 1)] = buffer[i];
        buffer[i] = temp;
    }

    if (neg)
        scia_msg("-");
    scia_msg(buffer);
}


SemaphoreHandle_t wii_ready = NULL;
static StaticSemaphore_t wiiReadyBuffer;


#define STACK_SIZE  256U


static StaticTask_t updateTaskBuffer;
static StackType_t  updateTaskStack[STACK_SIZE];


static StaticTask_t idleTaskBuffer;
static StackType_t  idleTaskStack[STACK_SIZE];

//bool bulletCollided(struct entinity entinity, struct entinity bullet)
//{
//    struct coord topLeft = {
//         .x = entinity.x - entinity.width/2,
//         .y = entinity.y + entinity.height/2
//    };
//
//    struct coord bottomRight = {
//         .x = entinity.x - entinity.width/2,
//         .y = entinity.y + entinity.height/2
//    };
//
//    if ((topLeft.x < bullet.x && bottomRight.x > bullet.x) &&
//        (topLeft.y > bullet.y && bottomRight.y <bullet.y))
//      return true;
//
//    return false;
//}


void updateTask(void * pvParameters)
{
    int waiting = false;
    while (1)
    {
        scia_msg("\rupdate\n");
        while (I2CA_WriteData(&I2cReq) != I2C_SUCCESS);
        vTaskDelay(100 / portTICK_PERIOD_MS);
        while (I2CA_ReadData(&I2cMsgIn1) != I2C_SUCCESS);
        vTaskDelay(100 / portTICK_PERIOD_MS);
        char data[6];
        int i;
        for(i=0; i < 6; i++)
            data[i] = (I2caRegs.I2CDRR.all ^ 0x17) + 0x17;

        int zPressed = data[5] & 0x01;
        int cPressed = (data[5] >> 1) & 0x01;

        scia_msg("\n\rX: ");
        //printNum()
        scia_msg("\n\rY: ");
        //printNum

        scia_msg("\n\rZ-Button: ");
        printNum(zPressed);
        scia_msg("\n");
        scia_msg("\n\rC-Button: ");
        printNum(!cPressed && !zPressed);
        scia_msg("\n");



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

void updateRemote()
{

}


//-------------------------------------------------------------------------------------------------
void vApplicationStackOverflowHook( TaskHandle_t xTask, signed char *pcTaskName )
{
    while(1);
}

//-------------------------------------------------------------------------------------------------
void vApplicationSetupTimerInterrupt( void )
{
    // Start the timer than activate timer interrupt to switch into first task.
    EALLOW;
    PieVectTable.TIMER2_INT = &portTICK_ISR;
    EDIS;

    ConfigCpuTimer(&CpuTimer2,
                   configCPU_CLOCK_HZ / 1000000,  // CPU clock in MHz
                   1000000 / configTICK_RATE_HZ); // Timer period in uS
    CpuTimer2Regs.TCR.all = 0x4000;               // Enable interrupt and start timer
    IER |= M_INT14;
}

//-------------------------------------------------------------------------------------------------
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
    *ppxIdleTaskTCBBuffer = &idleTaskBuffer;
    *ppxIdleTaskStackBuffer = idleTaskStack;
    *pulIdleTaskStackSize = STACK_SIZE;
}

//-------------------------------------------------------------------------------------------------
void main(void)
{
    // Step 1. Initialize System Control:
    // PLL, WatchDog, enable Peripheral Clocks
    // This example function is found in the F2837xS_SysCtrl.c file.
    InitSysCtrl();

    // Step 2. Initialize GPIO:
    // This example function is found in the F2837xS_Gpio.c file and
    // illustrates how to set the GPIO to it's default state.
    InitCpuTimers();

    InitGpio();

    GPIO_SetupPinMux(105, GPIO_MUX_CPU1, 1);
    GPIO_SetupPinMux(104, GPIO_MUX_CPU1, 1);


    // Step 3. Clear all interrupts and initialize PIE vector table:
    // Disable CPU interrupts
    DINT;

    // Initialize the PIE control registers to their default state.
    // The default state is all PIE interrupts disabled and flags
    // are cleared.
    // This function is found in the F2837xS_PieCtrl.c file.
    InitPieCtrl();

    // Disable CPU interrupts and clear all CPU interrupt flags:
    IER = 0x0000;
    IFR = 0x0000;

    InitPieVectTable();

    // Initialize the PIE vector table with pointers to the shell Interrupt
    // Service Routines (ISR).
    // This will populate the entire table, even if the interrupt
    // is not used in this example.  This is useful for debug purposes.
    // The shell ISR routines are found in F2837xS_DefaultIsr.c.
    // This function is found in F2837xS_PieVect.c.
    InitPieVectTable();


    I2CA_Init();
    uart_init();
    scia_msg("Starting up..");


    // Enable global Interrupts and higher priority real-time debug events:
    EINT;  // Enable Global interrupt INTM
    ERTM;  // Enable Global realtime interrupt DBGM

    wii_ready = xSemaphoreCreateBinaryStatic( &wiiReadyBuffer );

    // Create the task without using any dynamic memory allocation.
    xTaskCreateStatic(updateTask,           // Function that implements the task.
                      "Update task",        // Text name for the task.
                      STACK_SIZE,           // Number of indexes in the xStack array.
                      ( void * ) 0,       // Parameter passed into the task.
                      tskIDLE_PRIORITY + 2, // Priority at which the task is created.
                      updateTaskStack,      // Array to use as the task's stack.
                      &updateTaskBuffer );  // Variable to hold the task's data structure.

    while (I2CA_WriteData(&I2cInit) != I2C_SUCCESS);
    while (I2CA_WriteData(&I2cReq) != I2C_SUCCESS);

    vTaskStartScheduler();
}

//
// I2CA_Init - Initialize I2CA settings
//
void I2CA_Init(void)
{
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
