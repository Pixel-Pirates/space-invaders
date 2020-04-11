///*
// * lcd.c
// *
// *  Created on: Apr 4, 2020
// *      Author: Raymond Bernardo
// */
//
#include "../bsp/bsp.h"
#include "../libs/printNum.h"
#include "../bsp/device_driver/fatfs/src/tff.h"
#include "../game.h"
#include "../libs/sprite.h"

typedef struct bmp
{
    FIL file;
    char* src;
    int startAddress;
} bmp_t;


extern SemaphoreHandle_t sd_ready;
extern SemaphoreHandle_t player_ready;

extern invader_t invaders[27];
extern player_t player;
static bmp_t invader_a;
static bmp_t invader_b;
static bmp_t player_bmp;
extern bool gameOver;

uint16_t bmp_read_16(bmp_t* bmp, unsigned short* usBytesRead);
void bmp_open(bmp_t* bmp, char* src);
void bmp_read(bmp_t* bmp, char * data, int len, unsigned short* usBytesRead);

char invaderAPixels[INVADER_SIZE];
char playerPixels[PLAYER_SIZE];
char invaderBPixels[INVADER_SIZE];

void invaderTask() {

    unsigned short usBytesRead;
    bmp_open(&invader_a, "invaderA.bmp");
    bmp_open(&invader_b, "invaderB.bmp");
    bmp_open(&player_bmp, "shooter.bmp");
    bmp_read(&player_bmp, playerPixels, PLAYER_SIZE, &usBytesRead);
    player.sprite.data = playerPixels;


    bmp_read(&invader_a, invaderAPixels, INVADER_SIZE, &usBytesRead);
    bmp_read(&invader_b, invaderBPixels, INVADER_SIZE, &usBytesRead);
    char* invaderPixels = invaderAPixels;
    int movingRight = true;
    int hitEnd = false;
    bool aActive = true;
    int count = 0;

    while (1) {
        while(gameOver);
        for (int i = 0; i < INVADER_COLUMNS*INVADER_ROWS; i++)
        {
            invader_t* invader = &invaders[i];
            if (!invader->alive) continue;



            if (aActive) {
                invader->sprite.data = invaderAPixels;
            }
            else {
                invader->sprite.data = invaderBPixels;
            }
            sprite_draw(&invader->sprite);



            if (movingRight) {
                invader->sprite.x += 1;
                if (invader->sprite.x + invader->sprite.width > MAX_SCREEN_X - 1)
                {
                    hitEnd = true;
                }
            } else {
                invader->sprite.x -= 1;

                if (invader->sprite.x <= 0)
                {
                    hitEnd = true;
                }
            }

        }

        if (hitEnd) {
            hitEnd = false;
            movingRight = !movingRight;
        }
        if (count++ > 10){
            aActive = !aActive;
            count = 0;
        }

        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}

uint16_t bmp_read_16(bmp_t* bmp, unsigned short* usBytesRead)
{
    uint16_t data[2];
    xSemaphoreTake(sd_ready, portMAX_DELAY);
    f_read(&bmp->file, data, 2, usBytesRead);
    xSemaphoreGive(sd_ready);
    return data[0] | (data[1] << 8);
}

void bmp_open(bmp_t* bmp, char* src)
{
    xSemaphoreTake(sd_ready, portMAX_DELAY);
    FRESULT fresult = f_open(&bmp->file, src, FA_READ);
    if(fresult != FR_OK) scia_msg("\rDid not open\n");

    f_lseek(&bmp->file, 0xA);
    if(fresult != FR_OK) scia_msg("\rDid not seek\n");

    unsigned short usBytesRead;
    uint16_t data[4];
    fresult = f_read(&bmp->file, data, 4, &usBytesRead);

    uint32_t startAddress = data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);

    fresult = f_lseek(&bmp->file, startAddress);
    if(fresult != FR_OK) scia_msg("\rDid not seek\n");
    xSemaphoreGive(sd_ready);
    bmp->startAddress = startAddress;
}

void bmp_read(bmp_t* bmp, char * data, int len, unsigned short* usBytesRead)
{
    xSemaphoreTake(sd_ready, portMAX_DELAY);
    f_read(&bmp->file, data, len, usBytesRead);
    xSemaphoreGive(sd_ready);
}

