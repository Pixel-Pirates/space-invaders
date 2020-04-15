/*
 * music.c
 *
 *  Created on: Mar 21, 2020
 *      Author: Raymond Bernardo
 */


#include "../bsp/bsp.h"
#ifdef SPEAKER
#include "../libs/printNum.h"
#include "../bsp/device_driver/fatfs/src/tff.h"
//#include "dac.h"
//#include "driverlib.h"
#include "../game.h"
#include <F28x_Project.h>

#define BUFFER_SIZE 100

typedef struct wav
{
    FIL file;
    int startAddress;
} wav_t;

extern SemaphoreHandle_t music_ready;
extern SemaphoreHandle_t sd_ready;
extern volatile bool gameOver;
bool playerShootSound = false;

void wav_open(wav_t* wav, char* src);
void wav_start(wav_t* wav);
void wav_read(wav_t* wav, uint16_t * data, int len, unsigned short* usBytesRead);


uint16_t ping[BUFFER_SIZE];
uint16_t pong[BUFFER_SIZE];
uint16_t* out;
uint16_t* in;
bool ready = false;
int counter = 0;
void sampleTimer()
{

#ifdef CONTROLSUITE
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
#else
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
#endif

    if (counter >= BUFFER_SIZE - 1)
    {
        if (!ready)
        {
            return;
        }

        counter = 0;
        uint16_t* temp = out;
        out = in;
        in = temp;
        ready = false;
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xSemaphoreGiveFromISR(music_ready, &xHigherPriorityTaskWoken);
    }

#ifdef CONTROLSUITE
    DacaRegs.DACVALS.all = out[counter];
#else
    DAC_setShadowValue(DACA_BASE,  out[counter]);
#endif
    counter++;
}

void speakerTask()
{
    wav_t title;
    wav_t shoot;
    wav_open(&title, "title.wav");
    wav_open(&shoot, "shoot.wav");


    out = ping;
    in = pong;
    clearBuffer(out, BUFFER_SIZE);
    clearBuffer(in, BUFFER_SIZE);


    //Set up ADC
    EALLOW;
    DacaRegs.DACCTL.bit.DACREFSEL = 1;
    DacaRegs.DACCTL.bit.LOADMODE = 0;
    DacaRegs.DACOUTEN.bit.DACOUTEN = 1;
    DacaRegs.DACVALS.all = 0;
    EDIS;

    //Set up timer for samples
    ConfigCpuTimer(&CpuTimer0,
                   configCPU_CLOCK_HZ / 1000000,  // CPU clock in MHz
                   1000000 / 16000); // Timer period in uS

#ifdef CONTROLSUITE

    CpuTimer0Regs.TCR.bit.TIE = 1;
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;
    CpuTimer0Regs.TCR.bit.TSS = 0;
    EnableInterrupts();
#else
    CPUTimer_enableInterrupt(CPUTIMER0_BASE);
    Interrupt_enable(INT_TIMER0);
    CPUTimer_startTimer(CPUTIMER0_BASE);
#endif

    EINT;
    ERTM;

    unsigned short usBytesRead;

    while (1)
    {
        clearBuffer(in, BUFFER_SIZE);

        if (gameOver) {
            wav_read(&title, in, BUFFER_SIZE - 1, &usBytesRead);
            if (usBytesRead == 0)
            {
                wav_start(&title);
            }
        }

        if (playerShootSound) {
            wav_read(&shoot, in, BUFFER_SIZE - 1, &usBytesRead);
            if (usBytesRead == 0)
            {
                wav_start(&shoot);
                playerShootSound = false;
            }
        }


        ready = true;

        xSemaphoreTake(music_ready, portMAX_DELAY);
    }
}

void clearBuffer(uint16_t * data, int len) {
    for (int i = 0; i < len; i++) {
        data[i] = 0;
    }
}

void wav_start(wav_t* wav){
    xSemaphoreTake(sd_ready, portMAX_DELAY);
    f_lseek(&wav->file, wav->startAddress);
    xSemaphoreGive(sd_ready);
}

void wav_read(wav_t* wav, uint16_t * data, int len, unsigned short* usBytesRead)
{
    xSemaphoreTake(sd_ready, portMAX_DELAY);
    f_read(&wav->file, data, len, usBytesRead);
    xSemaphoreGive(sd_ready);
}

void wav_open(wav_t* wav, char* src) {
    //SPI-C
    //CS - Select GPIO125 (J2 12)
    //DO - MISO GPIO123 (J2 18)
    //SCK - CLK GPIO124 (J2 13)
    //DI - MOSI GPIO122 (J2 17)
    //CD - VCC
    xSemaphoreTake(sd_ready, portMAX_DELAY);
    FRESULT fresult = f_open(&wav->file, src, FA_READ);
    xSemaphoreGive(sd_ready);

    if(fresult != FR_OK) return scia_msg("\rDid not open\n");

    unsigned short usBytesRead;
    int i = 0;
    uint16_t addr = 0;
    char data[1];

    //data section starts with "data" find it
    while (i < 4) {
        xSemaphoreTake(sd_ready, portMAX_DELAY);
        fresult = f_read(&wav->file, data, 1, &usBytesRead);
        xSemaphoreGive(sd_ready);

        if (usBytesRead == 0) {
            return scia_msg("\rCould not find data.\n");
        }

        char character = data[0];
        if ((i == 0 && character == 'd') ||
            (i == 1 && character == 'a') ||
            (i == 2 && character == 't') ||
            (i == 3 && character == 'a')) {
           i++;
        } else {
            i = 0;
        }
        addr++;
    }
    wav->startAddress = addr;
}

#endif
