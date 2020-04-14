/*
 * bmp.h
 *
 *  Created on: Apr 14, 2020
 *      Author: Raymond Bernardo
 */

#ifndef LIBS_BMP_H_
#define LIBS_BMP_H_

typedef struct bmp
{
    FIL file;
    char* src;
    int startAddress;
} bmp_t;

uint16_t bmp_read_16(bmp_t* bmp, unsigned short* usBytesRead);
void bmp_open(bmp_t* bmp, char* src);
void bmp_read(bmp_t* bmp, char * data, int len, unsigned short* usBytesRead);


#endif /* LIBS_BMP_H_ */
