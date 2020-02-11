/*
 * printNum.c
 *
 *  Created on: Feb 11, 2020
 *      Author: Raymond Bernardo
 */

#include "../bsp/bsp.h"

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
        buffer[index++] = 0x30;

    while (num > 0)
    {
        buffer[index++] = (num % 10) + 0x30;
        num /= 10;
    }

    int size = index;
    buffer[size] = 0;

    int i;
    for (i = 0; i < size/2; i++)
    {
        char temp = buffer[size - (i + 1)];
        buffer[size - (i + 1)] = buffer[i];
        buffer[i] = temp;
    }

    if (neg)
        scia_msg("-");
    scia_msg(buffer);
}

