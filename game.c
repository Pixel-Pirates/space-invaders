/*
 * game.c
 *
 *  Created on: Jan 17, 2020
 *      Author: Raymond Bernardo
 */
/* Kernel includes. */
#include "FreeRTOS.h" /* Must come first. */
#include "task.h"     /* RTOS task related API prototypes. */
#include "queue.h"    /* RTOS queue related API prototypes. */
#include "timers.h"   /* Software timer related API prototypes. */
#include "semphr.h"   /* Semaphore related API prototypes. */

#include "game.h"
#include "stdbool.h"


#include "F28x_Project.h"     // Device Headerfile and Examples Include File



#define STACK_SIZE  256U


static StaticTask_t updateTaskBuffer;
static StackType_t  updateTaskStack[STACK_SIZE];

static StaticTask_t idleTaskBuffer;
static StackType_t  idleTaskStack[STACK_SIZE];

bool bulletCollided(struct entinity entinity, struct entinity bullet)
{
    struct coord topLeft = {
         .x = entinity.x - entinity.width/2,
         .y = entinity.y + entinity.height/2
    };

    struct coord bottomRight = {
         .x = entinity.x - entinity.width/2,
         .y = entinity.y + entinity.height/2
    };

    if ((topLeft.x < bullet.x && bottomRight.x > bullet.x) &&
        (topLeft.y > bullet.y && bottomRight.y <bullet.y))
      return true;

    return false;
}

void updateTask(void * pvParameters)
{
    while (1)
    {
        vTaskDelay(16 / portTICK_PERIOD_MS);



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


    // Enable global Interrupts and higher priority real-time debug events:
    EINT;  // Enable Global interrupt INTM
    ERTM;  // Enable Global realtime interrupt DBGM

    // Create the task without using any dynamic memory allocation.
    xTaskCreateStatic(updateTask,           // Function that implements the task.
                      "Update task",        // Text name for the task.
                      STACK_SIZE,           // Number of indexes in the xStack array.
                      ( void * ) 0,       // Parameter passed into the task.
                      tskIDLE_PRIORITY + 2, // Priority at which the task is created.
                      updateTaskStack,      // Array to use as the task's stack.
                      &updateTaskBuffer );  // Variable to hold the task's data structure.

    vTaskStartScheduler();
}
