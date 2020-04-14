
#ifndef GAME_H_
#define GAME_H_

//#define VGA 0
#define LCD 1

//#define RAW 0
#define BMP 1

//#define SPEAKER 1

#define INVADER_ROWS 3
#define INVADER_COLUMNS 7

//#define INVADER_ROWS 1
//#define INVADER_COLUMNS 1

#define INVADER_HEIGHT 30
#define INVADER_WIDTH 22
#define INVADER_SIZE INVADER_WIDTH*INVADER_HEIGHT*2

#define PLAYER_HEIGHT 31
#define PLAYER_WIDTH 30
#define PLAYER_SIZE PLAYER_WIDTH*PLAYER_HEIGHT*2

typedef struct sprite
{
    char* src; //image source
    FIL file; //sd card file
    char * data;
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
} invader_t;

typedef struct player
{
    sprite_t sprite;
    int lives;
} player_t;

#endif /* THREADS_LCD_H_ */
