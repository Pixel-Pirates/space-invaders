/*
 * game.c
 *
 *  Created on: Jan 17, 2020
 *      Author: Raymond Bernardo
 */
/* Kernel includes. */



#include "game.h"
#include "bsp/bsp.h"
#include "threads/updateTask.h"
#include "bsp/device_driver/fatfs/src/tff.h"

SemaphoreHandle_t wii_ready = NULL;
static StaticSemaphore_t wiiReadyBuffer;


#define STACK_SIZE  512U

static StaticTask_t updateTaskBuffer;
static StackType_t  updateTaskStack[STACK_SIZE];

static StaticTask_t idleTaskBuffer;
static StackType_t  idleTaskStack[STACK_SIZE];

static FATFS g_sFatFs;
static FIL g_sFileObject;

void __error__(char *pcFilename, unsigned long ulLine)
{
    ESTOP0;
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


    BSP_Init();


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


//    vTaskStartScheduler();

        FRESULT fresult = f_mount(0, &g_sFatFs);
        if(fresult != FR_OK) scia_msg("\rDid not mount\n");

        fresult = f_open(&g_sFileObject, "wtf.txt", FA_READ);
        if(fresult != FR_OK) scia_msg("\rDid not open\n");

        fresult = f_read(&g_sFileObject, g_cTmpBuf, sizeof(g_cTmpBuf) - 1,
                                 &usBytesRead);

        while(1);
}

