/*
 * music.c
 *
 *  Created on: Mar 21, 2020
 *      Author: Raymond Bernardo
 */

/*
 * updateTask.c
 *
 *  Created on: Feb 11, 2020
 *      Author: Raymond Bernardo
 */

#include "../bsp/bsp.h"
#include "../libs/printNum.h"
#include "../bsp/device_driver/fatfs/src/tff.h"
#include "dac.h"
#include "driverlib.h"

static FATFS g_sFatFs;
static FIL g_sFileObject;

#define BUFFER_SIZE 100
uint16_t ping[BUFFER_SIZE];
uint16_t pong[BUFFER_SIZE];
uint16_t* out;
uint16_t* in;
bool ready = false;
int counter = 0;
void sampleTimer()
{
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);

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
    }

    DAC_setShadowValue(DACA_BASE,  out[counter]);
    counter++;
}

void updateSpeaker()
{
    //SPI-C
    //CS - Select GPIO125 (J2 12)
    //DO - MISO GPIO123 (J2 18)
    //SCK - CLK GPIO124 (J2 13)
    //DI - MOSI GPIO122 (J2 17)
    //CD - VCC
    scia_msg("\rOpen SD Card\n");
    FRESULT fresult = f_mount(0, &g_sFatFs);
    if(fresult != FR_OK) scia_msg("\rDid not mount\n");


    fresult = f_open(&g_sFileObject, "title.wav", FA_READ);
    if(fresult != FR_OK) scia_msg("\rDid not open\n");

    f_lseek(&g_sFileObject, 44);
    if(fresult != FR_OK) scia_msg("\rDid not seek\n");

    out = ping;
    in = pong;

    unsigned short usBytesRead;
    fresult = f_read(&g_sFileObject, out, BUFFER_SIZE - 1, &usBytesRead);

    EALLOW;
    DacaRegs.DACCTL.bit.DACREFSEL = 1;
    DacaRegs.DACCTL.bit.LOADMODE = 0;
    DacaRegs.DACOUTEN.bit.DACOUTEN = 1;
    DacaRegs.DACVALS.all = 0;
    EDIS;

//    EALLOW;
//    PieVectTable.TIMER1_INT = &sampleTimer;
//    EDIS;
    ConfigCpuTimer(&CpuTimer0,
                   configCPU_CLOCK_HZ / 1000000,  // CPU clock in MHz
                   1000000 / 16000); // Timer period in uS
    CPUTimer_enableInterrupt(CPUTIMER0_BASE);
    Interrupt_enable(INT_TIMER0);
    CPUTimer_startTimer(CPUTIMER0_BASE);
    EINT;
    ERTM;

    //int waiting = false;
    while (1)
    {
        if (ready)
        {
            continue;
        }

        fresult = f_read(&g_sFileObject, in, BUFFER_SIZE - 1, &usBytesRead);
        ready = true;
        if (usBytesRead == 0)
        {
            f_lseek(&g_sFileObject, 0x4A);
        }
    }
}


//void updateSpeaker()
//{
//    //SPI-C
//    //CS - Select GPIO125 (J2 12)
//    //DO - MISO GPIO123 (J2 18)
//    //SCK - CLK GPIO124 (J2 13)
//    //DI - MOSI GPIO122 (J2 17)
//    //CD - VCC
//    scia_msg("\rOpen SD Card\n");
//    FRESULT fresult = f_mount(0, &g_sFatFs);
//    if(fresult != FR_OK) scia_msg("\rDid not mount\n");
//
//
//    fresult = f_open(&g_sFileObject, "1000hz.wav", FA_READ);
//    if(fresult != FR_OK) scia_msg("\rDid not open\n");
//
//    f_lseek(&g_sFileObject, 0x4A);
//    if(fresult != FR_OK) scia_msg("\rDid not seek\n");
//
//    out = ping;
//    in = pong;
//    uint32_t len = 0;
//    unsigned short usBytesRead;
//
//    do
//    {
//        fresult = f_read(&g_sFileObject, out, BUFFER_SIZE - 1, &usBytesRead);
//        int c;
//        for (c = 0; c < usBytesRead; c++)
//        {
//            sram_write(len, out[c]);
//            len++;
//        }
//
//        if (len >= 500000) break;
//
//    } while (usBytesRead != 0);
//
////    for (uint32_t addr = 0; addr < 1024; addr++)
////    {
////        for (uint32_t i = 0; i < 256; i++)
////        {
////            sram_write(addr, i);
////            addr++;
////            len++;
////        }
////
////        for (uint32_t i = 256; i > 0; i--)
////        {
////            sram_write(addr, i);
////            addr++;
////            len++;
////        }
////    }
//
//    EALLOW;
//    DacaRegs.DACCTL.bit.DACREFSEL = 1;
//    DacaRegs.DACCTL.bit.LOADMODE = 0;
//    DacaRegs.DACOUTEN.bit.DACOUTEN = 1;
//    DacaRegs.DACVALS.all = 0;
//    EDIS;
//
////    EALLOW;
////    PieVectTable.TIMER1_INT = &sampleTimer;
////    EDIS;
//    ConfigCpuTimer(&CpuTimer0,
//                   configCPU_CLOCK_HZ / 1000000,  // CPU clock in MHz
//                   1000000 / 16000); // Timer period in uS
//    CPUTimer_enableInterrupt(CPUTIMER0_BASE);
//    Interrupt_enable(INT_TIMER0);
//    CPUTimer_startTimer(CPUTIMER0_BASE);
//    EINT;
//    ERTM;
//
//    uint32_t addr = 0;
//    while (1)
//    {
//        if (ready)
//        {
//            continue;
//        }
//
//        int bufferIndex = 0;
//        for (bufferIndex = 0; bufferIndex < BUFFER_SIZE - 1; bufferIndex++)
//        {
//            uint16_t data = sram_read(addr);
//            in[bufferIndex] = data;
//            addr++;
//            if (addr >= len)
//            {
//                addr = 0;
//            }
//        }
//
//        ready = true;
//    }
//}
