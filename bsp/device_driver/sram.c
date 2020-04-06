/*
 * sram.c
 *
 *  Created on: Mar 28, 2020
 *      Author: Raymond Bernardo
 */
#include "../bsp.h"
#include "driverlib.h"

void OE_select()
{
    GPIO_writePin(56, 0);
}

void OE_deselect()
{
    GPIO_writePin(56, 1);
}

void WE_select()
{
    GPIO_writePin(139, 0);
}

void WE_deselect()
{
    GPIO_writePin(139, 1);
}


#define PORTA_MASK (((uint32_t)1 << 14) | ((uint32_t)1 << 15) | ((uint32_t)1 << 16) | ((uint32_t)1 << 22) | ((uint32_t)1 << 24) | ((uint32_t)1 << 25) | ((uint32_t)1 << 26) | ((uint32_t)1 << 27) | ((uint32_t)1 << 29))
#define PORTB_MASK (((uint32_t)1 << (41 - 32)) | ((uint32_t)1 << (63 - 32)) | ((uint32_t)1 << (52 - 32)) | ((uint32_t)1 << (52 - 32)) | ((uint32_t)1 << (40 - 32)))
#define PORTC_MASK (((uint32_t)1 << 31) | ((uint32_t)1 << 30) | ((uint32_t)1 << 3) | ((uint32_t)1 << 2) | ((uint32_t)1 << 1) | ((uint32_t)1 << 0))

void set_address(uint32_t addr)
{
    //GPIO14 - GPIO16 | A0 - A2
    uint32_t portA = (addr & 0b111)  << 14;
    //GPIO22 | A3
    portA |= (addr & 0b1000) << (22 - 3);
    //GPIO24 - GPIO27 | A4 - A7
    portA |= (addr & 0b11110000) << (24 - 4);
    //GPIO29 | A8
    portA |= (addr & 0b100000000) << (29 - 8);
    GPIO_setPortPins(GPIO_PORT_A, portA);
    GPIO_clearPortPins(GPIO_PORT_A, (~portA) & PORTA_MASK);

    // GPIO97 - A9
    GPIO_writePin(97, (addr >> 9) & 1);

    uint32_t portC = 0;
    // GPIO95 - A10
    portC |= (addr & ((uint32_t)1 << 10)) << ((95 - 64) - 10);
    // GPIO94 - A11
    portC |= (addr & ((uint32_t)1 << 11)) << ((94 - 64) - 11);
    // GPIO67 - A12
    portC |= (addr & ((uint32_t)1 << 12)) >> (12 - (67 - 64));
    // GPIO66 - A13
    portC |= (addr & ((uint32_t)1 << 13)) >> (13 - (66 - 64));


    uint32_t portB = 0;
    // GPIO41 - A14
    portB |= (addr & ((uint32_t)1 << 14)) >> (14 - (41 - 32));
    // GPIO65 - A15
    portC |= (addr & ((uint32_t)1 << 15)) >> (15 - (65 - 64));
    // GPIO64 - A16
    portC |= (addr & ((uint32_t)1 << 16)) >> (16 - (64 - 64));
    // GPIO63 - A17
    portB |= (addr & ((uint32_t)1 << 17)) << ((63 - 32) - 17);
    // GPIO52 - A18
    portB |= (addr & ((uint32_t)1 << 18)) << ((52 - 32) - 18);
    // GPIO40 - A19
    portB |= (addr & ((uint32_t)1 << 19)) >> (19 - (40 - 32));


    GPIO_setPortPins(GPIO_PORT_B, portB);
    GPIO_clearPortPins(GPIO_PORT_B, (~portB) & PORTB_MASK);

    GPIO_setPortPins(GPIO_PORT_C, portC);
    GPIO_clearPortPins(GPIO_PORT_C, (~portC) & PORTC_MASK);
}

uint16_t get_data()
{
    return GPIO_readPortData(GPIO_PORT_A) & 0x7FF;
}

void set_data(uint16_t data)
{
    GPIO_setPortPins(GPIO_PORT_A, data & 0x7FF);
    GPIO_clearPortPins(GPIO_PORT_A, (~data) & 0x7FF);
}

void set_data_in()
{
    GPIO_setDirectionMode(0, GPIO_DIR_MODE_IN);    // GPIO0 - D0
    GPIO_setDirectionMode(1, GPIO_DIR_MODE_IN);    // GPIO1 - D1
    GPIO_setDirectionMode(2, GPIO_DIR_MODE_IN);    // GPIO2 - D2
    GPIO_setDirectionMode(3, GPIO_DIR_MODE_IN);    // GPIO3 - D3
    GPIO_setDirectionMode(4, GPIO_DIR_MODE_IN);    // GPIO4 - D4
    GPIO_setDirectionMode(5, GPIO_DIR_MODE_IN);    // GPIO5 - D5
    GPIO_setDirectionMode(6, GPIO_DIR_MODE_IN);    // GPIO6 - D6
    GPIO_setDirectionMode(7, GPIO_DIR_MODE_IN);    // GPIO7 - D7
}

void set_data_out()
{
    GPIO_setDirectionMode(0, GPIO_DIR_MODE_OUT);    // GPIO0 - D0
    GPIO_setDirectionMode(1, GPIO_DIR_MODE_OUT);    // GPIO1 - D1
    GPIO_setDirectionMode(2, GPIO_DIR_MODE_OUT);    // GPIO2 - D2
    GPIO_setDirectionMode(3, GPIO_DIR_MODE_OUT);    // GPIO3 - D3
    GPIO_setDirectionMode(4, GPIO_DIR_MODE_OUT);    // GPIO4 - D4
    GPIO_setDirectionMode(5, GPIO_DIR_MODE_OUT);    // GPIO5 - D5
    GPIO_setDirectionMode(6, GPIO_DIR_MODE_OUT);    // GPIO6 - D6
    GPIO_setDirectionMode(7, GPIO_DIR_MODE_OUT);    // GPIO7 - D7
}

bool outputIn = false;

uint16_t sram_read(uint32_t addr)
{
    //set_address(addr);
    //GPIO14 - GPIO16 | A0 - A2
    uint32_t portA = (addr & 0b111)  << 14;
    //GPIO22 | A3
    portA |= (addr & 0b1000) << (22 - 3);
    //GPIO24 - GPIO27 | A4 - A7
    portA |= (addr & 0b11110000) << (24 - 4);
    //GPIO29 | A8
    portA |= (addr & 0b100000000) << (29 - 8);
    GPIO_setPortPins(GPIO_PORT_A, portA);
    GPIO_clearPortPins(GPIO_PORT_A, (~portA) & PORTA_MASK);

    // GPIO97 - A9
    GPIO_writePin(97, (addr >> 9) & 1);

    uint32_t portC = 0;
    // GPIO95 - A10
    portC |= (addr & ((uint32_t)1 << 10)) << ((95 - 64) - 10);
    // GPIO94 - A11
    portC |= (addr & ((uint32_t)1 << 11)) << ((94 - 64) - 11);
    // GPIO67 - A12
    portC |= (addr & ((uint32_t)1 << 12)) >> (12 - (67 - 64));
    // GPIO66 - A13
    portC |= (addr & ((uint32_t)1 << 13)) >> (13 - (66 - 64));


    uint32_t portB = 0;
    // GPIO41 - A14
    portB |= (addr & ((uint32_t)1 << 14)) >> (14 - (41 - 32));
    // GPIO65 - A15
    portC |= (addr & ((uint32_t)1 << 15)) >> (15 - (65 - 64));
    // GPIO64 - A16
    portC |= (addr & ((uint32_t)1 << 16)) >> (16 - (64 - 64));
    // GPIO63 - A17
    portB |= (addr & ((uint32_t)1 << 17)) << ((63 - 32) - 17);
    // GPIO52 - A18
    portB |= (addr & ((uint32_t)1 << 18)) << ((52 - 32) - 18);
    // GPIO40 - A19
    portB |= (addr & ((uint32_t)1 << 19)) >> (19 - (40 - 32));


    GPIO_setPortPins(GPIO_PORT_B, portB);
    GPIO_clearPortPins(GPIO_PORT_B, (~portB) & PORTB_MASK);

    GPIO_setPortPins(GPIO_PORT_C, portC);
    GPIO_clearPortPins(GPIO_PORT_C, (~portC) & PORTC_MASK);

    //set_data_in();
    if (!outputIn) {
        GPIO_setDirectionMode(0, GPIO_DIR_MODE_IN);    // GPIO0 - D0
        GPIO_setDirectionMode(1, GPIO_DIR_MODE_IN);    // GPIO1 - D1
        GPIO_setDirectionMode(2, GPIO_DIR_MODE_IN);    // GPIO2 - D2
        GPIO_setDirectionMode(3, GPIO_DIR_MODE_IN);    // GPIO3 - D3
        GPIO_setDirectionMode(4, GPIO_DIR_MODE_IN);    // GPIO4 - D4
        GPIO_setDirectionMode(5, GPIO_DIR_MODE_IN);    // GPIO5 - D5
        GPIO_setDirectionMode(6, GPIO_DIR_MODE_IN);    // GPIO6 - D6
        GPIO_setDirectionMode(7, GPIO_DIR_MODE_IN);    // GPIO7 - D7
        outputIn = true;
    }
    //OE_select();
    GPIO_writePin(56, 0);
    //F28x_usDelay(1);
    //return get_data();
     return GPIO_readPortData(GPIO_PORT_A) & 0x7FF;
}

void sram_write(uint32_t addr, uint16_t data)
{
    set_address(addr);
    OE_deselect();
    WE_select();
    set_data_out();
    set_data(data);
    F28x_usDelay(1);
    WE_deselect();
}

void sram_init()
{
    GPIO_setDirectionMode(0, GPIO_DIR_MODE_IN);    // GPIO0 - D0
    GPIO_setDirectionMode(1, GPIO_DIR_MODE_IN);    // GPIO1 - D1
    GPIO_setDirectionMode(2, GPIO_DIR_MODE_IN);    // GPIO2 - D2
    GPIO_setDirectionMode(3, GPIO_DIR_MODE_IN);    // GPIO3 - D3
    GPIO_setDirectionMode(4, GPIO_DIR_MODE_IN);    // GPIO4 - D4
    GPIO_setDirectionMode(5, GPIO_DIR_MODE_IN);    // GPIO5 - D5
    GPIO_setDirectionMode(6, GPIO_DIR_MODE_IN);    // GPIO6 - D6
    GPIO_setDirectionMode(7, GPIO_DIR_MODE_IN);    // GPIO7 - D7

    GPIO_setDirectionMode(14, GPIO_DIR_MODE_OUT);    // GPIO14 - A0
    GPIO_setDirectionMode(15, GPIO_DIR_MODE_OUT);    // GPIO15 - A1
    GPIO_setDirectionMode(16, GPIO_DIR_MODE_OUT);    // GPIO16 - A2
    GPIO_setDirectionMode(22, GPIO_DIR_MODE_OUT);    // GPIO22 - A3
    GPIO_setDirectionMode(24, GPIO_DIR_MODE_OUT);    // GPIO24 - A4
    GPIO_setDirectionMode(25, GPIO_DIR_MODE_OUT);    // GPIO25 - A5
    GPIO_setDirectionMode(26, GPIO_DIR_MODE_OUT);    // GPIO26 - A6
    GPIO_setDirectionMode(27, GPIO_DIR_MODE_OUT);    // GPIO27 - A7
    GPIO_setDirectionMode(29, GPIO_DIR_MODE_OUT);    // GPIO29 - A8
    GPIO_setDirectionMode(97, GPIO_DIR_MODE_OUT);    // GPIO97 - A9
    GPIO_setDirectionMode(95, GPIO_DIR_MODE_OUT);    // GPIO95 - A10
    GPIO_setDirectionMode(94, GPIO_DIR_MODE_OUT);    // GPIO94 - A11
    GPIO_setDirectionMode(67, GPIO_DIR_MODE_OUT);    // GPIO67 - A12
    GPIO_setDirectionMode(66, GPIO_DIR_MODE_OUT);    // GPIO66 - A13
    GPIO_setDirectionMode(41, GPIO_DIR_MODE_OUT);    // GPIO41 - A14
    GPIO_setDirectionMode(65, GPIO_DIR_MODE_OUT);    // GPIO65 - A15
    GPIO_setDirectionMode(64, GPIO_DIR_MODE_OUT);    // GPIO64 - A16
    GPIO_setDirectionMode(63, GPIO_DIR_MODE_OUT);    // GPIO63 - A17
    GPIO_setDirectionMode(52, GPIO_DIR_MODE_OUT);    // GPIO52 - A18
    GPIO_setDirectionMode(40, GPIO_DIR_MODE_OUT);    // GPIO40 - A19

    GPIO_setDirectionMode(139, GPIO_DIR_MODE_OUT);    // GPIO139 - WE
    GPIO_setDirectionMode(56, GPIO_DIR_MODE_OUT);    // GPIO56 - OE
    OE_deselect();
    WE_deselect();

//    scia_msg("TEST - READ WRITE\n\r");

//    uint32_t i;
//    for (i = 0; i < ((uint32_t)1 << 20); i++)
//    {
//        sram_write(i, (uint16_t)i + (i % 50));
//    }
//
//    int incorrect = 0;
//    for (i = 0; i < ((uint32_t)1 << 20); i++)
//    {
//        if (sram_read(i) != ((uint16_t)i + (i % 50)) & 0xFF)
//        {
//            //printNum((i & 0xff) + (i % 50));
//            //scia_msg(" != ");
//            //printNum(sram_read(i));
//            //scia_msg(" Incorrect read!\n\r");
//            incorrect++;
//        }
//    }
//    scia_msg("Incorrect: ");
//    printNum(incorrect);
//    scia_msg("\n\r");
//
//    scia_msg("Total: ");
//    printNum((uint32_t)1 << 20);
//    scia_msg("\n\r");
}

