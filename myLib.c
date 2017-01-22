#include "myLib.h"
#include <stdio.h>
#include <stdlib.h>

unsigned short *videoBuffer = (unsigned short *)0x6000000;



//  DRAWING FUNCTIONS ================================================

/*
 * Sets a single pixel the given color
 */
void setPixel(int row, int col, unsigned short color) {
    videoBuffer[OFFSET(row,col, 240)] = color;
}

/*
 * Sets a single pixel the given color but only in Playing Area
 */
void setSafePixel(int row, int col, unsigned short color) {
    if (row > GBA_SCREEN_HEIGHT - STATUS_BAR_HEIGHT - 1) {
        return;
    }
    if (row < 0) {
        return;
    }
    videoBuffer[OFFSET(row,col, 240)] = color;
}

/*
 * Uses DMA to quickly draw rectangles
 */
void drawRect(int row, int col, int height, int width, volatile unsigned short color)
{
    for(int r=0; r<height; r++)
    {
        REG_DMA3SAD = (u32)&color;
        REG_DMA3DAD = (u32)(&videoBuffer[OFFSET(row+r, col, 240)]);
        REG_DMA3CNT = width | DMA_ON | DMA_SOURCE_FIXED;
    }
}

/*
 * Uses DMA to quickly draw rectangles only in the playing area
 */
void drawSafeRect(int row, int col, int height, int width, volatile unsigned short color)
{
    if (row > GBA_SCREEN_HEIGHT - STATUS_BAR_HEIGHT) {
        return;
    }
    if (row + height > GBA_SCREEN_HEIGHT - STATUS_BAR_HEIGHT) {
        height = GBA_SCREEN_HEIGHT - STATUS_BAR_HEIGHT - row;
    }
    if (row < 0) {
        if (row + height >= 0) {
            int temp = row + height;
            row = 0;
            height = temp;
        } else {
            return;
        }
    }
    if (col > GBA_SCREEN_WIDTH - 1) {
        return;
    } else if (col + width > GBA_SCREEN_WIDTH - 1) {
        width = GBA_SCREEN_WIDTH - col;
    }
    if (col + width < 0) {
        return;
    } else if (col < 0) {
        col = 0;
        width = col + width;
    }
    for(int r = row; r < row + height; r++)
    {
        REG_DMA3SAD = (u32)&color;
        REG_DMA3DAD = (u32)(&videoBuffer[OFFSET(r, col, 240)]);
        REG_DMA3CNT = width | DMA_ON | DMA_SOURCE_FIXED;
    }
}

/**
 * Draws the image to the screen
 */
void drawImage3(int row, int col, int height, int width, const unsigned short *image) {

    for(int r=0; r<height; r++)
    {
        
        REG_DMA3SAD = (u32) (image + r*width);
        REG_DMA3DAD = (u32)(videoBuffer + row*240 + r*240 + col);
        REG_DMA3CNT = width | DMA_ON;
        
    }

}

/**
 * Draws the image avoiding status bar area and above row 0
 */
void drawSafeImage3(int row, int col, int height, int width, const unsigned short *image) {

    if (row > GBA_SCREEN_HEIGHT - STATUS_BAR_HEIGHT) {
        return;
    }
    if (row + height > GBA_SCREEN_HEIGHT - STATUS_BAR_HEIGHT) {
        height = GBA_SCREEN_HEIGHT - STATUS_BAR_HEIGHT - row;
    }
    if (row < 0) {
        if (row + height >= 0) {
            int temp = row + height;
            row = 0;
            height = temp;
        } else {
            return;
        }
    }

    if (col > GBA_SCREEN_WIDTH - 1) {
        return;
    } else if (col + width > GBA_SCREEN_WIDTH - 1) {
        width = GBA_SCREEN_WIDTH - col;
    }
    if (col + width < 0) {
        return;
    }
    
    for(int r=0; r<height; r++)
    {

        REG_DMA3SAD = (u32) (image + r*width);
        REG_DMA3DAD = (u32)(videoBuffer + row*240 + r*240 + col);
        REG_DMA3CNT = width | DMA_ON;
        
    }

}

/*
 * "Pauses" processing until Vertical Blank
 */
void waitForVblank()
{
    while(SCANLINECOUNTER > 160);
    while(SCANLINECOUNTER < 160);
}

/*
 * Clears the screen to background color
 */
void clearScreen() {
    REG_DMA3SAD = (u32)&bgcolor;
    REG_DMA3DAD = (u32)videoBuffer;
    REG_DMA3CNT = 38400 | DMA_ON | DMA_SOURCE_FIXED;
}

/*
 * Draws a char at the selected location in the given color
 */
void drawChar(int row, int col, char ch, u16 color)
{
    for(int r=0; r<8; r++)
    {
        for(int c=0; c<6; c++)
        {
            if(fontdata_6x8[OFFSET(r, c, 6)+ch*48])
            {
                setPixel(row+r, col+c, color);
            }
        }
    }
}

/*
 * Draws the given string at the given location
 */
void drawString(int row, int col, char *str, u16 color)
{
    while(*str)
    {
        drawChar(row, col, *str++, color);
        col += 6;
        
    }
}

/*
 * Draws "Level: " and the current level number in the middle
 * of the screen. Then waits for a short time and erases the level number.
 */
void drawLevelNumber(int levelNum) {
    char buffer[STATUS_LEVEL_CHARS];
    sprintf(buffer, "LEVEL: %2i", levelNum);
    drawString(70, 90, buffer, YELLOW);
    for (volatile int i = 0; i < 500000; i++);
    drawSafeRect(60, 90, 100, 100, bgcolor);
}

/*
 * Draws the Status Bar at the bottom of the screen.
 */
void initializeStatusBar(int lives, int aliens, int level) {

    // draw top bar
    drawRect(STATUS_BAR_ROW, STATUS_BAR_COL, 1, STATUS_BAR_WIDTH, STATUS_BAR_COLOR);

    // draw bottom bar
    drawRect(STATUS_BAR_ROW + STATUS_BAR_HEIGHT - 1, STATUS_BAR_COL, 1, STATUS_BAR_WIDTH, STATUS_BAR_COLOR);

    // draw left bar
    drawRect(STATUS_BAR_ROW, STATUS_BAR_COL, STATUS_BAR_HEIGHT, 1, STATUS_BAR_COLOR);

    // draw right bar
    drawRect(STATUS_BAR_ROW, STATUS_BAR_COL + STATUS_BAR_WIDTH, STATUS_BAR_HEIGHT, 1, STATUS_BAR_COLOR);

    // draw two middle vertical bars
    drawRect(STATUS_BAR_ROW, STATUS_BAR_COL + STATUS_ALIENS_COL - 1, STATUS_BAR_HEIGHT, 1, STATUS_BAR_COLOR);
    drawRect(STATUS_BAR_ROW, STATUS_BAR_COL + STATUS_LEVEL_COL - 1, STATUS_BAR_HEIGHT, 1, STATUS_BAR_COLOR);


    // draw lives
    char buffer1[STATUS_SHIPS_CHARS];
    sprintf(buffer1, "SHIPS: %i", lives);
    drawString(STATUS_TEXT_ROW, STATUS_SHIPS_COL, buffer1, YELLOW);

    // draw Score
    char buffer2[STATUS_ALIENS_CHARS];
    sprintf(buffer2, "ALIENS: %3i", aliens);
    drawString(STATUS_TEXT_ROW, STATUS_ALIENS_COL, buffer2, YELLOW);

    // draw level
    char buffer3[STATUS_LEVEL_CHARS];
    sprintf(buffer3, "LEVEL: %2i", level);
    drawString(STATUS_TEXT_ROW, STATUS_LEVEL_COL, buffer3, YELLOW);

}

/*
 * Updates the number of remaining ships on the status bar
 */
void updateLives(int lives) {

    // erase old lives
    drawRect(STATUS_TEXT_ROW, STATUS_SHIPS_COL, STATUS_CHAR_HEIGHT, STATUS_CHAR_WIDTH * STATUS_SHIPS_CHARS, bgcolor);

    // draw new lives
    char buffer[STATUS_SHIPS_CHARS];
    sprintf(buffer, "SHIPS: %i", lives);
    drawString(STATUS_TEXT_ROW, STATUS_SHIPS_COL, buffer, YELLOW);
}

/*
 * Updates the number of aliens destroyed on the status bar
 */
void updateAliens(int aliens) {

    // erase old lives
    drawRect(STATUS_TEXT_ROW, STATUS_ALIENS_COL, STATUS_CHAR_HEIGHT, STATUS_CHAR_WIDTH * STATUS_ALIENS_CHARS, bgcolor);

    // draw new lives
    char buffer[STATUS_ALIENS_CHARS];
    sprintf(buffer, "ALIENS: %3i", aliens);
    drawString(STATUS_TEXT_ROW, STATUS_ALIENS_COL, buffer, YELLOW);
}

/*
 * Updates the level number on the status bar
 */
void updateLevel(int level) {

    // erase old lives
    drawRect(STATUS_TEXT_ROW, STATUS_LEVEL_COL, STATUS_CHAR_HEIGHT, STATUS_CHAR_WIDTH * STATUS_LEVEL_CHARS, bgcolor);

    // draw new lives
    char buffer[STATUS_LEVEL_CHARS];
    sprintf(buffer, "LEVEL: %2i", level);
    drawString(STATUS_TEXT_ROW, STATUS_LEVEL_COL, buffer, YELLOW);
}




// COLLISION DETECTION ==============================================

/*
 * Checks for a collision between two rectangles
 */
int isCollision(int Arow, int Acol, int Awidth, int Aheight,
                int Brow, int Bcol, int Bwidth, int Bheight) {

    return !( (Arow + Aheight - 1) < (Brow) ||
              (Arow) > (Brow + Bheight - 1) ||
              (Acol) > (Bcol + Bwidth - 1)  ||
              (Acol + Awidth - 1) < (Bcol)      );
} 

/*
 * Checks for collisions between aliens and the ship
 */
int checkShipAlienCollisions(SHIP *ship, ALIEN *alien, int *numAliens) {
    int collisionOcurred = 0;
    for (int i = 0; i < *numAliens; i++) {
        if (ship->exist && ship->willExist &&
            isCollision(alien[i].row, alien[i].col, alien[i].height, alien[i].width,
                                        ship->row, ship->col, ship->height, ship->width)) {

            alien[i].willExist = 0;
            ship->willExist = 0;
            collisionOcurred = 1;
        }
    }
    return collisionOcurred;
}

/*
 * Checks for collisions between aliens and shots
 */
void checkAlienShotCollisions(ALIEN *alien, int *numAliens, SHOT *shot, int *numShots, int *aliensDestroyed) {
    for (int i = 0; i < *numAliens; i++) {
        if (alien[i].exist && alien[i].willExist) {
            for (int j = 0; j < *numShots; j++) {
                if (shot[j].exist && shot[j].willExist) {
                    if (isCollision(alien[i].row, alien[i].col, alien[i].height, alien[i].width,
                                    shot[j].row, shot[j].col, shot[j].height, shot[j].width)) {
                                                
                        alien[i].willExist = 0;
                        shot[j].willExist = 0;
                        (*aliensDestroyed) += 1;
                        updateAliens(*aliensDestroyed);

                    }
                } 
            }
        }
    }
}



// CALCULATING POSITIONS ===========================================

/*
 * Calculates the ships position based on user input
 */
void calculateShipPosition(SHIP *ship) {

    // Ship Movement Left
    if(KEY_DOWN_NOW(BUTTON_LEFT)) {
        if (ship->col > 0 + ship->colDelta) {
            ship->col -= ship->colDelta;
        } else if (ship->col > 0 ) {
            ship->col = 0;
        }
    }

    // Ship Movement Right
    if(KEY_DOWN_NOW(BUTTON_RIGHT)) {
        if (ship->col < 240 - ship->width - ship->colDelta) {
            ship->col += ship->colDelta;
        } else if (ship->col < 240) {
            ship->col = 240 - ship->width;
        }
    }
}

/*
 * Calculates if a new shot is being made by the user.
 */
void calculateNewShot(SHOT *shot, volatile int *numShots, SHIP *ship, int shotWasPressed) {
    
    // New Shot
    if(KEY_DOWN_NOW(BUTTON_UP) && !shotWasPressed && *numShots < TOTAL_NUM_SHOTS
            && (*ship).exist && (*ship).willExist) {
                            
        SHOT newShot = {SHOT_START_ROW, (*ship).col + ((*ship).width / 2), SHOT_START_ROW, SHOT_ROW_DELTA, SHOT_WIDTH, SHOT_HEIGHT, SHOT_EXIST, SHOT_WILL_EXIST, SHOT_COLOR};
        shot[*numShots] = newShot;
        (*numShots)++;
        
    }
}

/*
 * Calculates the position of user's shot
 */
void calculateShotPositions(SHOT *shot, volatile int *numShots) {
    
    // iterate through each shot and calculate new positions
    for(int i = 0; i < *numShots; i++) {   
        if (shot[i].exist) {
            // change position
            shot[i].row = shot[i].row - shot[i].rowDelta;
        }
    }
}

/*
 * Calculates the alien's position
 */
void calculateAlienPositions(ALIEN *alien, int numAliens, int shipCol) {

    for (int i=0; i<numAliens; i++) {
        if (alien[i].exist) {
           
            switch(alien[i].type) {

            case TYPE_A:

                // alien goes to user's ship
                if (alien[i].col < shipCol) {
                    alien[i].col = alien[i].col + alien[i].colDelta;
                } else if (alien[i].col > shipCol) {
                    alien[i].col = alien[i].col - alien[i].colDelta;
                }

                // alien goes to top of screen after flying below sreen
                if (alien[i].row > 160) {
                    alien[i].row = 0 - alien[i].height;
                }

                // alien continues flying down
                alien[i].row = alien[i].row + alien[i].rowDelta;
                break;

            case TYPE_B:

                // alien goes to home location
                if (alien[i].moveCounter < 100 + alien[i].randNum) {
                    goToLocation(&alien[i], HOME_ROW(alien[i].number), HOME_COL(alien[i].number, (NUM_ALIENS_LEVEL < TOTAL_NUM_ALIENS ? NUM_ALIENS_LEVEL : TOTAL_NUM_ALIENS)));

                // alien flies to bottom of screen
                } else if (alien[i].row < GBA_SCREEN_HEIGHT - STATUS_BAR_HEIGHT) {
                    alien[i].row = alien[i].row + alien[i].rowDelta;
                    if (alien[i].moveCounter % 100 < 50 && alien[i].col < GBA_SCREEN_WIDTH - alien[i].width) {
                        alien[i].col = alien[i].col + alien[i].colDelta;
                    } else if (alien[i].col > 0) {
                        alien[i].col = alien[i].col - alien[i].colDelta;
                    }

                // alien returns to home location
                } else if (alien[i].row >= GBA_SCREEN_HEIGHT - STATUS_BAR_HEIGHT) {
                    goToLocation(&alien[i], (int)HOME_ROW(alien[i].number), (int)calculateHomeColumn(alien[i].number, NUM_ALIENS_LEVEL));
                    alien[i].moveCounter = 0;
                }

                // increment moveCounter
                alien[i].moveCounter += 1;

                // reset counter if too high
                if (alien[i].moveCounter > 2000) {
                    alien[i].moveCounter = 0;
                }

                break;
            }
        }
    }

}

/*
 * Helper function for calculateAlienPositions. This calculates an alien's home
 * column based on it's unique number. This needs some refactoring!!
 */
int calculateHomeColumn(int alienNumber, int maxNumberAliens) {
    return (ALIEN_WIDTH + (alienNumber % 8)*((GBA_SCREEN_WIDTH - ALIEN_WIDTH) / ((maxNumberAliens <= 8 ? maxNumberAliens : (alienNumber > 8 ? (maxNumberAliens % 8) : (8))))));
}

/*
 * Helper function for calculateAlienPositions. This causes the alien to go
 * to the selected location.
 */
void goToLocation(ALIEN *alien, int row, int col) {

    if (alien->col < col) {
        alien->col = alien->col + alien->colDelta;
    } else if (alien->col > col) {
        alien->col = alien->col - alien->colDelta;
    }

    if (alien->row < row) {
        alien->row = alien->row + alien->rowDelta;
    } else if (alien->row > row) {
        alien->row = alien->row - alien->rowDelta;
    }

}



// ERASING and DESTROYING Old Objects

/*
 * Erases the user's ship. Also changes ship's status to not exist if willExist is false.
 * Might want to put this section aspect in its own method...
 */
void eraseOldShip(SHIP *ship) {
    drawSafeRect(ship->row, ship->oldCol - ship->colDelta, ship->height, ship->width + 2*ship->colDelta, bgcolor);
    if (!ship->willExist) {
        ship->exist = 0;
    }
}

/*
 * Erases old shots
 */
void eraseOldShots(SHOT *shot, int numShots) {
    
    for (int i = 0; i < numShots; i++) {

        // erase shot
        drawSafeRect(shot[i].oldRow, shot[i].col, shot[i].height + 2*shot[i].rowDelta, shot[i].width, bgcolor);
    }
}

/*
 * Checks if shots should still be drawn or not
 */
void shouldShotsExist(SHOT *shot, int *numShots) {
    for (int i = 0; i < *numShots; i++) {
        
        // if shot goes beyond screen
        if (shot[i].row < 0 - shot[i].height) {
            shot[i].willExist = 0;  
        }

        // if shot will not exist
        if (!shot[i].willExist) {
            shotDestroyed(&shot[0], i, numShots);
            i--;
        }   
    }
}

/*
 * Properly disposes of a shot no longer in the game. This can be made quicker
 * by turning shots into a hybrid queue array instead of arraylist. Returns
 * index of current shot in array.
 */
void shotDestroyed(SHOT *shot, int index, volatile int *numShots) {

    shot[index].exist = 0;
    for (int i = index; i < *numShots - 1; i++) {
        shot[i] = shot[i+1];
    }
    (*numShots)--;
}

/*
 * Erases old aliens images
 */
void eraseOldAliens(ALIEN *alien, int *numAliens) {
    
    for (int i = 0; i < *numAliens; i++) {
        

            // erasing alien
            drawSafeRect(alien[i].oldRow - alien[i].rowDelta, alien[i].oldCol - alien[i].colDelta, alien[i].height + 2*alien[i].rowDelta, alien[i].width + 2*alien[i].colDelta, bgcolor);

            if (!alien[i].willExist) {
                alien[i].exist = 0;
                alienDestroyed(&alien[0], i, numAliens);
            }
        
    }
}

/*
 * Properly disposes of alien when destroyed
 */
void alienDestroyed(ALIEN *alien, int index, int *numAliens) {

    for (int i = index; i < *numAliens; i++) {
        alien[i] = alien[i + 1];
    }
    (*numAliens)--;
}
