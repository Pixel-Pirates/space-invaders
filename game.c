/*
 * game.c
 *
 *  Created on: Jan 17, 2020
 *      Author: Raymond Bernardo
 */
/* Kernel includes. */




#include "bsp/bsp.h"
#include "bsp/device_driver/fatfs/src/tff.h"
#include "game.h"
#include "libs/sprite.h"
#include "threads/thread.h"

#ifdef VGA
    #include "libs/VGA.h"
#endif


//Mutxes
SemaphoreHandle_t wii_ready;
SemaphoreHandle_t lcd_ready;
SemaphoreHandle_t player_ready;
SemaphoreHandle_t sd_ready;

//binary
SemaphoreHandle_t music_ready;
SemaphoreHandle_t bullet_ready;

static StaticSemaphore_t wiiReadyBuffer;
static StaticSemaphore_t lcdReadyBuffer;
static StaticSemaphore_t musicReadyBuffer;
static StaticSemaphore_t playerReadyBuffer;
static StaticSemaphore_t sdReadyBuffer;
static StaticSemaphore_t bulletReadyBuffer;

invader_t invaders[INVADER_COLUMNS*INVADER_ROWS];
player_t player;
bool firstRun = true;

#define STACK_SIZE  1024U

#pragma DATA_SECTION(updateBombStack,"ramgs2")
#pragma DATA_SECTION(updateBulletStack,"ramgs2")
#pragma DATA_SECTION(updateTaskStack,"ramgs2")
#pragma DATA_SECTION(updateSpeakerStack,"ramgs3")
#pragma DATA_SECTION(updateLcdStack,"ramgs3")
#pragma DATA_SECTION(idleTaskStack,"ramgs3")

static StaticTask_t updateBombBuffer;
static StackType_t  updateBombStack[STACK_SIZE];

static StaticTask_t updateBulletBuffer;
static StackType_t  updateBulletStack[STACK_SIZE];

static StaticTask_t updateTaskBuffer;
static StackType_t  updateTaskStack[STACK_SIZE];

static StaticTask_t updateSpeakerBuffer;
static StackType_t  updateSpeakerStack[STACK_SIZE];

static StaticTask_t updateLcdBuffer;
static StackType_t  updateLcdStack[STACK_SIZE];

static StaticTask_t idleTaskBuffer;
static StackType_t  idleTaskStack[STACK_SIZE];

extern volatile bool gameOver = true;

void setUpGame();

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
    //IER = 0x0000;
    //IFR = 0x0000;

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

    wii_ready = xSemaphoreCreateMutexStatic( &wiiReadyBuffer );
    sd_ready = xSemaphoreCreateMutexStatic( &sdReadyBuffer );
    player_ready = xSemaphoreCreateMutexStatic( &playerReadyBuffer );
    lcd_ready = xSemaphoreCreateMutexStatic( &lcdReadyBuffer );

    music_ready = xSemaphoreCreateBinaryStatic( &musicReadyBuffer );
    bullet_ready = xSemaphoreCreateBinaryStatic( &bulletReadyBuffer );

#ifdef VGA
    GPIO_SetupPinOptions(32, GPIO_OUTPUT, 0);
#endif

    setUpGame();

#ifdef VGA
    drawBackGround();
#endif

    // Create the task without using any dynamic memory allocation.
    xTaskCreateStatic(updateTask,           // Function that implements the task.
                      "Update task",        // Text name for the task.
                      STACK_SIZE,           // Number of indexes in the xStack array.
                      ( void * ) 0,       // Parameter passed into the task.
                      tskIDLE_PRIORITY + 2, // Priority at which the task is created.
                      updateTaskStack,      // Array to use as the task's stack.
                      &updateTaskBuffer );  // Variable to hold the task's data structure.
#ifdef SPEAKER
    xTaskCreateStatic(speakerTask,           // Function that implements the task.
                          "Seaker task",        // Text name for the task.
                          STACK_SIZE,           // Number of indexes in the xStack array.
                          ( void * ) 0,       // Parameter passed into the task.
                          tskIDLE_PRIORITY + 2, // Priority at which the task is created.
                          updateSpeakerStack,      // Array to use as the task's stack.
                          &updateSpeakerBuffer );  // Variable to hold the task's data structure.
#endif

    xTaskCreateStatic(invaderTask,           // Function that implements the task.
                      "Invader task",        // Text name for the task.
                      STACK_SIZE,           // Number of indexes in the xStack array.
                      ( void * ) 0,       // Parameter passed into the task.
                      tskIDLE_PRIORITY + 2, // Priority at which the task is created.
                      updateLcdStack,      // Array to use as the task's stack.
                      &updateLcdBuffer );  // Variable to hold the task's data structure.


    xTaskCreateStatic(bulletTask,           // Function that implements the task.
                          "bullet",        // Text name for the task.
                          STACK_SIZE,           // Number of indexes in the xStack array.
                          ( void * ) 0,       // Parameter passed into the task.
                          tskIDLE_PRIORITY + 2, // Priority at which the task is created.
                          updateBulletStack,      // Array to use as the task's stack.
                          &updateBulletBuffer );  // Variable to hold the task's data structure.

    xTaskCreateStatic(bombTask,           // Function that implements the task.
                              "bomb",        // Text name for the task.
                              STACK_SIZE,           // Number of indexes in the xStack array.
                              ( void * ) 0,       // Parameter passed into the task.
                              tskIDLE_PRIORITY + 2, // Priority at which the task is created.
                              updateBombStack,      // Array to use as the task's stack.
                              &updateBombBuffer );  // Variable to hold the task's data structure.

    vTaskStartScheduler();



    while(1);
}


void setUpGame()
{
    int i = 0;
    for (int yIndex = 0; yIndex < INVADER_ROWS; yIndex++)
    {
        for (int xIndex = 0; xIndex < INVADER_COLUMNS; xIndex++)
        {
            invader_t invader = invaders[i];
            invader.alive = true;
            invader.sprite.x = xIndex*(INVADER_WIDTH + 10) + 10;
            invader.sprite.y = yIndex*(INVADER_HEIGHT + 10) + 10;
            invader.sprite.width = INVADER_HEIGHT;
            invader.sprite.height = INVADER_WIDTH;
            invader.sprite.undraw = false;
            invaders[i] = invader;
            i++;
        }
    }

    player.lives = 3;
    player.sprite.height = PLAYER_HEIGHT;
    player.sprite.width = PLAYER_WIDTH;
    player.sprite.undraw = true;

    player.sprite.x = 50;
    player.sprite.y = MAX_SCREEN_Y - PLAYER_WIDTH;

#ifdef VGA
    if(firstRun)
        firstRun = false;
    else
        clearScreen();
    loadPlayer();
#endif
}
