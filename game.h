
#ifndef GAME_H_
#define GAME_H_

#include "env.h"

#define INVADER_ROWS 5
#define INVADER_COLUMNS 11


//#define INVADER_ROWS 3
//#define INVADER_COLUMNS 7

//#define INVADER_ROWS 1
//#define INVADER_COLUMNS 1

#ifdef SMALL_SPRITES
    #define INVADER_HEIGHT 16
    #define INVADER_WIDTH 9

    #define PLAYER_HEIGHT 8
    #define PLAYER_WIDTH 19
#else
    #define INVADER_HEIGHT 30
    #define INVADER_WIDTH 22

    #define PLAYER_HEIGHT 31
    #define PLAYER_WIDTH 30
#endif

#define INVADER_SIZE INVADER_WIDTH*INVADER_HEIGHT*2
#define PLAYER_SIZE PLAYER_WIDTH*PLAYER_HEIGHT*2

#define PLAYER_START_X (320-PLAYER_WIDTH)/2

#define VGA_OFFSET_X 160
#define VGA_OFFSET_Y 179

#define SPRITE_OFFSET_Y         20
#define VGA_SPRITE_OFFSET_Y     SPRITE_OFFSET_Y + VGA_OFFSET_Y

#define HEADER_SCORE_TEXT   7
#define HEADER_SCORE_NUM    HEADER_SCORE_TEXT + 48
#define HEADER_LIVES_TEXT   HEADER_SCORE_TEXT + 144
#define HEADER_LIVES_NUM    HEADER_LIVES_TEXT + 48
#define HEADER_Y            3

typedef enum{
    SMALL = 30,
    MEDIUM = 20,
    LARGE = 10,
    UNDEF = 0xFF
}invaderSize;

typedef struct sprite
{
    char* src; //image source
    FIL file; //sd card file
    #ifdef ONBOARD
        uint16_t * data;
    #else
        char * data;
    #endif
    bool loaded;
    int32_t x;
    int32_t y;
    int32_t _x; //previous x
    int32_t _y; //previous y
    uint32_t width;
    uint32_t height;
    bool undraw;
} sprite_t;

typedef struct invader
{
    sprite_t sprite;
    bool alive;
    bool justDied;
    int deadFrames;
    invaderSize type;
} invader_t;

typedef struct player
{
    sprite_t sprite;
    int lives;
} player_t;

#endif /* THREADS_LCD_H_ */
