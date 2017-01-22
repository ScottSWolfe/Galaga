// myLib.h

// Images
#include "GalagaTitleScreen.h"
#include "GameOverScreen.h"
#include "ship_image.h"
#include "AlienTypeA.h"
#include "AlienTypeB.h"

// Types
typedef unsigned short u16;
typedef unsigned int u32;

// Drawing and Text
extern const unsigned char fontdata_6x8[12288];
void drawChar(int row, int col, char ch, u16 color);
void drawString(int row, int col, char *str, u16 color);
void drawSafeRect(int row, int col, int height, int width, u16 color);
void setSafePixel(int row, int col, u16 color);
void drawImage3(int x, int y, int width, int height, const unsigned short *image);
void drawSafeImage3(int x, int y, int width, int height, const unsigned short *image);
void drawLevelNumber(int levelNum);
void updateLives(int lives);
void updateAliens(int aliens);
void updateLevel(int level);
void setPixel(int row, int col, unsigned short color);
void drawRect(int row, int col, int height, int width, unsigned short color);
void waitForVblank();
void initializeStatusBar();
void clearScreen();


// GBA Display Controls
#define REG_DISPCTL *(unsigned short *)0x4000000
#define MODE3 3
#define BG2_ENABLE (1<<10)

// GBA Screen
#define GBA_SCREEN_WIDTH 240
#define GBA_SCREEN_HEIGHT 160

// Display Stuff
#define SCANLINECOUNTER *(volatile unsigned short *)0x4000006
extern unsigned short *videoBuffer;
#define OFFSET(row, col, rowlen)  ((row)*(rowlen)+(col))
extern u16 bgcolor;

// Colors
#define COLOR(r, g, b) ((r) | (g)<<5 | (b)<<10)
#define WHITE COLOR(31,31,31)
#define RED COLOR(31,0,0)
#define GREEN COLOR(0,31,0)
#define BLUE COLOR(0,0,31)
#define MAGENTA COLOR(31, 0, 31)
#define CYAN COLOR(0, 31, 31)
#define YELLOW COLOR(31, 31, 0)
#define GREY COLOR(25, 25, 25)
#define BLACK 0

// Buttons
#define BUTTON_A        (1<<0)
#define BUTTON_B        (1<<1)
#define BUTTON_SELECT   (1<<2)
#define BUTTON_START    (1<<3)
#define BUTTON_RIGHT    (1<<4)
#define BUTTON_LEFT     (1<<5)
#define BUTTON_UP       (1<<6)
#define BUTTON_DOWN     (1<<7)
#define BUTTON_R        (1<<8)
#define BUTTON_L        (1<<9)

#define KEY_DOWN_NOW(key)  (~(BUTTONS) & key)
#define BUTTONS *(volatile unsigned int *)0x4000130

// DMA
#define REG_DMA0SAD         *(volatile u32*)0x40000B0       // source address
#define REG_DMA0DAD         *(volatile u32*)0x40000B4       // destination address
#define REG_DMA0CNT         *(volatile u32*)0x40000B8       // control register

// DMA channel 1 register definitions
#define REG_DMA1SAD         *(volatile u32*)0x40000BC       // source address
#define REG_DMA1DAD         *(volatile u32*)0x40000C0       // destination address
#define REG_DMA1CNT         *(volatile u32*)0x40000C4       // control register

// DMA channel 2 register definitions
#define REG_DMA2SAD         *(volatile u32*)0x40000C8       // source address
#define REG_DMA2DAD         *(volatile u32*)0x40000CC       // destination address
#define REG_DMA2CNT         *(volatile u32*)0x40000D0       // control register

// DMA channel 3 register definitions
#define REG_DMA3SAD         *(volatile u32*)0x40000D4       // source address
#define REG_DMA3DAD         *(volatile u32*)0x40000D8       // destination address
#define REG_DMA3CNT         *(volatile u32*)0x40000DC       // control register

typedef struct
{
    const volatile void *src;
    volatile void *dst;
    unsigned int cnt;
} DMA_CONTROLLER;

#define DMA ((volatile DMA_CONTROLLER *) 0x040000B0)

#define DMA_CHANNEL_0 0
#define DMA_CHANNEL_1 1
#define DMA_CHANNEL_2 2
#define DMA_CHANNEL_3 3

#define DMA_DESTINATION_INCREMENT (0 << 21)
#define DMA_DESTINATION_DECREMENT (1 << 21)
#define DMA_DESTINATION_FIXED (2 << 21)

#define DMA_SOURCE_INCREMENT (0 << 23)
#define DMA_SOURCE_DECREMENT (1 << 23)
#define DMA_SOURCE_FIXED (2 << 23)

#define DMA_REPEAT (1 << 25)

#define DMA_16 (0 << 26)
#define DMA_32 (1 << 26)

#define DMA_NOW (0 << 28)
#define DMA_AT_VBLANK (1 << 28)
#define DMA_AT_HBLANK (2 << 28)
#define DMA_AT_REFRESH (3 << 28)

#define DMA_IRQ (1 << 30)
#define DMA_ON (1 << 31)

// Collision Checking
int isCollision(int Arow, int Acol, int Awidth, int Aheight,
                int Brow, int Bcol, int Bwidth, int Bheight);



// STRUCTURES

// SHIP
typedef struct {
    int row;
    int col;
    int oldCol;
    int colDelta;
    int width;
    int height;
    int exist;
    int willExist;
} SHIP;

#define SHIP_START_ROW (160 - SHIP_HEIGHT - STATUS_BAR_HEIGHT - 2)
#define SHIP_START_COL ((240 - SHIP_WIDTH) / 2)
#define SHIP_COL_DELTA 2

#ifndef SHIPIMAGE_WIDTH
#define SHIPIMAGE_WIDTH 9
#define SHIPIMAGE_HEIGHT 12
#endif

#define SHIP_WIDTH SHIPIMAGE_WIDTH
#define SHIP_HEIGHT SHIPIMAGE_HEIGHT

#define SHIP_EXIST 1
#define SHIP_WILL_EXIST 1

#define RESPAWN_COUNTER_NUM 50

void calculateShipPosition(SHIP *ship);
void eraseOldShip(SHIP *ship);



// SHOT
typedef struct {
    int row;
    int col;
    int oldRow;
    int rowDelta;
    int width;
    int height;
    int exist;
    int willExist;
    u16 color;
} SHOT;

#define SHOT_START_ROW (160 - SHIP_HEIGHT - 10 - 1)
#define SHOT_START_COL (ship.col + (ship.width / 2))
#define SHOT_ROW_DELTA 3
#define SHOT_WIDTH 1
#define SHOT_HEIGHT 6
#define SHOT_EXIST 1
#define SHOT_NOT_EXIST 0
#define SHOT_WILL_EXIST 1
#define SHOT_WILL_NOT_EXIST 0
#define SHOT_COLOR BLUE
#define TOTAL_NUM_SHOTS 20

void eraseOldShots(SHOT *shot, int numShots);
void shouldShotsExist(SHOT *shot, int *numShots);
void shotDestroyed(SHOT *shot, int index, volatile int *numShots);
void calculateShotPositions(SHOT *shot, volatile int *numShots);
void calculateNewShot(SHOT *shot, volatile int *numShots, SHIP *ship, int shotWasPressed);


// ALIEN
enum AlienType {
    TYPE_A,
    TYPE_B
};

typedef struct {
    int row;
    int col;
    int oldRow;
    int oldCol;
    int rowDelta;
    int colDelta;
    int width;
    int height;
    int exist;
    int willExist;
    enum AlienType type;
    int moveCounter;
    int randNum;
    int number;
    const u16 *image;
} ALIEN;

#define ALIEN_ROW_DELTA (status.level > 2 ? 1 + (rand() % 2) : 1)
#define ALIEN_COL_DELTA 1
#define ALIEN_WIDTH 8
#define ALIEN_HEIGHT 8
#define ALIEN_EXIST 1
#define ALIEN_NOT_EXIST 0
#define ALIEN_WILL_EXIST 1
#define ALIEN_WILL_NOT_EXIST 0
#define ALIEN_MOVE_COUNTER_START 0

#define ALIEN_IMG_TYPEA AlienTypeA
#define ALIEN_IMG_TYPEB AlienTypeB

#define ALIEN_RAND_NUM (rand() % 250)

#define ALIEN_SPAWN_COUNT_NUM 500
#define TOTAL_NUM_ALIENS 9

#define ALIEN_START_ROW -10 //(0 - ALIEN_HEIGHT)
#define ALIEN_START_COL (rand() % (GBA_SCREEN_WIDTH - ALIEN_WIDTH))
#define ALIEN_SPAWN_START_ROW -10

#define ALIEN_RAND_COL (rand() % (GBA_SCREEN_WIDTH - ALIEN_WIDTH))

#define NUM_ALIENS_START 5
#define NUM_ALIENS_LEVEL (NUM_ALIENS_START + (status.level / 2))

#define HOME_ROW(NUM) (10 + (NUM / 8)*20)
// I realize this is a mess and needs refactoring... SORRY!
#define HOME_COL(NUMBER, NUM_ALIENS) (ALIEN_WIDTH + (NUMBER % 8)*((GBA_SCREEN_WIDTH - ALIEN_WIDTH) / ((NUM_ALIENS <= 8 ? NUM_ALIENS : (NUMBER > 8 ? (NUM_ALIENS % 8) : (8))))))

int checkShipAlienCollisions(SHIP *ship, ALIEN *alien, int *numAliens);
void checkAlienShotCollisions(ALIEN *alien, int *numAliens, SHOT *shot, int *numShots, int *aliensDestroyed);
void eraseOldAliens(ALIEN *alien, int *numAliens);
void alienDestroyed(ALIEN *alien, int index, int *numAliens);
void calculateAlienPositions(ALIEN *alien, volatile int numAliens, int shipCol);
int calculateHomeColumn(int alienNumber, int maxNumberAliens);
void goToLocation(ALIEN *alien, int row, int col);


// STATUS BAR
typedef struct {
    int lives;
    int aliens;
    int level;
} STATUS;

extern STATUS status;

#define STATUS_START_LIVES 3
#define STATUS_START_ALIENS 0
#define STATUS_START_LEVEL 1

#define STATUS_BAR_WIDTH 239
#define STATUS_BAR_HEIGHT 11
#define STATUS_BAR_ROW 149
#define STATUS_BAR_COL 0
#define STATUS_BAR_COLOR RED

#define STATUS_TEXT_ROW 151

#define STATUS_SHIPS_COL 2
#define STATUS_ALIENS_COL 80
#define STATUS_LEVEL_COL 166

#define STATUS_CHAR_WIDTH 6
#define STATUS_CHAR_HEIGHT 8

#define STATUS_SHIPS_CHARS 8
#define STATUS_ALIENS_CHARS 11
#define STATUS_LEVEL_CHARS 9



// GAME STATES
// State enum definition
enum GameState {
    START,
    START_NODRAW,
    PLAY,
    GAME_OVER,
    GAME_OVER_NODRAW
};






//=================================================================================================
// broken code to be added later
/*
// if past level 4, spawn a second alien
                        if (status.level > 4 && numAliens < TOTAL_NUM_ALIENS) {
                            ALIEN newAlien = {ALIEN_START_ROW, ALIEN_START_COL, ALIEN_START_ROW, ALIEN_START_COL,
                                            ALIEN_ROW_DELTA, ALIEN_COL_DELTA, ALIEN_WIDTH, ALIEN_HEIGHT,
                                            ALIEN_EXIST, ALIEN_WILL_EXIST, TYPE_B, ALIEN_MOVE_COUNTER_START,
                                            ALIEN_RAND_NUM, rand() % 8, ALIEN_IMG_TYPEB};

                            alien[numAliens] = newAlien;
                            numAliens++;
                        }
*/

// STAR code not currently being used ==========================================

    //int starSpawnCounter = 20;
    //int starSpawnCeiling = 100;

/*
                // Initialize Star Array
                STAR star[TOTAL_NUM_STARS];
                volatile int numStars = 0;
                for (int i = 0; i < TOTAL_NUM_STARS; i++) {

                    STAR newStar = {STAR_RAND_ROW, STAR_RAND_COL, STAR_ROW_DELTA, 0,
                        STAR_RAND_SIZE, STAR_NOT_EXIST, STAR_WILL_NOT_EXIST,
                        NEAR, STAR_DEFAULT_COLOR};

                    newStar.oldRow = newStar.row;

                    star[i] = newStar;   
                }
                

                // "turn on" some stars
                for (int i = 0; i < NUM_STARS_START; i++) {
                    star[i].exist = 1;
                    star[i].willExist = 1;
                    star[i].waitTime = 0;
                    numStars++;
                }
                // "turn on some stars "
                for (int i = NUM_STARS_START; i < 2 * NUM_STARS_START; i++) {
                    star[i].exist = 1;
                    star[i].willExist = 1;
                    star[i].waitTime = 0;
                    star[i].row = star[i].row - GBA_SCREEN_HEIGHT / 2;
                    star[i].oldRow = star[i].row - GBA_SCREEN_HEIGHT / 2;
                    numStars++;
                }
                */


                //calculateStarPositions(&star[0], &numStars);                  



/*
                    // erase old stars
                    for (int i = 0; i < numStars; i++) {

                        for (int j = 0; j < star[i].size + 3; j++) {
                            for (int k = 0; k < star[i].size + 1; k++) {
                                setSafePixel(star[i].oldRow + j - 3, star[i].col + k - 1, bgcolor);
                            }
                        }

                        if (!star[i].willExist) {
                            star[i].exist = 0;
                            starDestroyed(&star[0], i, &numStars);
                        }

                        // if star goes beyond screen
                        if (star[i].row > GBA_SCREEN_HEIGHT - STATUS_BAR_HEIGHT) {
                            star[i].willExist = 0;  
                        }
                    }
                    */



                    /*
                    // star spawn counter
                    if (starSpawnCounter < STAR_SPAWN_COUNT_NUM + starSpawnCeiling) {
                        starSpawnCounter++;
                    } else if (numStars < TOTAL_NUM_STARS) {

                        STAR newStar = {STAR_START_ROW, STAR_START_COL, STAR_ROW_DELTA,  STAR_START_ROW,
                                        STAR_SIZE, STAR_EXIST, STAR_WILL_EXIST, 0, STAR_COLOR};

                        star[numStars] = newStar;
                        numStars++;

                        starSpawnCounter = 0;
                        starSpawnCeiling = rand() % STAR_SPAWN_MOD_FACTOR;
                    }
                    */


                    /*
                    // draw stars
                    for (int i = 0; i < numStars; i++) {

                        for (int j = 0; j < star[i].size; j++) {
                            for (int k = 0; k < star[i].size; k++) {
                                setSafePixel(star[i].row + j, star[i].col + k, STAR_DEFAULT_COLOR);
                            }
                        }
                        star[i].oldRow = star[i].row;
                    }
                    */
/*
void calculateStarPositions(STAR *star, volatile int *numStars) {

    for (int i = 0; i < *numStars; i++) {
        star[i].row = star[i].row + star[i].rowDelta;
    }
}
*/

/*
void starDestroyed(STAR *star, int index, volatile int *numStars) {

    for (int i = index; i < *numStars; i++) {
        star[i] = star[i + 1];
    }
    (*numStars)--;
}
*/

/*
typedef struct {
    int row;
    int col;
    int rowDelta;
    int oldRow;
    int size;
    int exist;
    int willExist;
    enum StarType type;
    u16 color;
    //struct STAR *next;
} STAR;

#define STAR_START_ROW -5
#define STAR_RAND_ROW (rand() % (GBA_SCREEN_HEIGHT - STATUS_BAR_HEIGHT - 1))
#define STAR_RAND_COL (rand() % (GBA_SCREEN_WIDTH - STAR_SIZE))

#define STAR_ROW_DELTA ((rand() % 2) + 1)
#define STAR_FAR_DELTA 0
#define STAR_MID_DELTA 1
#define STAR_NEAR_DELTA 2

#define STAR_RAND_SIZE ((rand() % 2)+1)
#define STAR_FAR_SIZE 1
#define STAR_MID_SIZE 1
#define STAR_NEAR_SIZE 2
#define STAR_SIZE STAR_RAND_SIZE

#define STAR_EXIST 1
#define STAR_NOT_EXIST 0
#define STAR_WILL_EXIST 1
#define STAR_WILL_NOT_EXIST 0

#define STAR_DEFAULT_COLOR WHITE
//extern const u16[] *star_colors = {RED, BLUE, WHITE};

#define TOTAL_NUM_STARS 50
#define NUM_STARS_START 15

#define STAR_SPAWN_COUNT_NUM 5
#define STAR_SPAWN_MOD_FACTOR 5
#define STAR_WAIT_TIME (rand() % 30)

*/

/*
void starDestroyed(STAR *star, int index, volatile int *numStars);
void calculateStarPositions(STAR *star, volatile int *numStars);
*/

/*
enum StarType {
    FAR,
    MID,
    NEAR
};
*/