// main.c
#include <stdio.h>
#include <stdlib.h>
#include "myLib.h"


u16 bgcolor = BLACK;
STATUS status = {STATUS_START_LIVES, STATUS_START_ALIENS, STATUS_START_LEVEL};


int main() {

	// Set Display Mode
	REG_DISPCTL = MODE3 | BG2_ENABLE;

	// Set Game State
    int startWasDown = 0;
    enum GameState state = START;

	// Initialize time counters
    int respawnCounter = 0;
    int alienSpawnCounter = 0;


	while(1)
	{
		waitForVblank();
		switch(state) {

			case START:

                // Draw Title Screen
				clearScreen();
                drawImage3(0, 0, GALAGATITLESCREEN_HEIGHT, GALAGATITLESCREEN_WIDTH, GalagaTitleScreen);
                
                // Change State
				state = START_NODRAW;
				break;

			case START_NODRAW:

                // Waiting for User to press Start Button
				if (!startWasDown && KEY_DOWN_NOW(BUTTON_START)) {

                    // Reset gamestate
					status.lives = STATUS_START_LIVES;
					status.aliens = STATUS_START_ALIENS;
					status.level = STATUS_START_LEVEL;

                    // Change State
					state = PLAY;
				}
				break;

			case GAME_OVER:
				clearScreen();
				drawImage3(0, 0, GAMEOVER_HEIGHT, GAMEOVER_WIDTH, GameOver);
				state = GAME_OVER_NODRAW;
				break;

			case GAME_OVER_NODRAW:

                // Waiting for user to press Start or Select
				if (KEY_DOWN_NOW(BUTTON_START)) {
					state = START;
				}
				if (KEY_DOWN_NOW(BUTTON_SELECT)) {
					state = START;
				}
				break;

            // MAIN Game Loop State
			case PLAY:

				// INITIALIZE LEVEL
				clearScreen();
				initializeStatusBar(status.lives, status.aliens, status.level);

                // Draw LEVEL NUMBER
                drawLevelNumber(status.level);

				// Initialize Ship
				SHIP ship = {SHIP_START_ROW, SHIP_START_COL, SHIP_START_COL, SHIP_COL_DELTA, SHIP_WIDTH, SHIP_HEIGHT, SHIP_EXIST, SHIP_WILL_EXIST};

				// Initialize array of shots
				SHOT shot[TOTAL_NUM_SHOTS];
				for (int i = 0; i < TOTAL_NUM_SHOTS; i++) {
					SHOT newShot = {SHOT_START_ROW, SHOT_START_COL, SHOT_START_ROW, SHOT_ROW_DELTA, SHOT_WIDTH, SHOT_HEIGHT, SHOT_NOT_EXIST, SHOT_WILL_NOT_EXIST, SHOT_COLOR};
					shot[i] = newShot;
				}
				int numShots = 0;
				volatile int shotWasPressed = 0;

				// Initialize array of Aliens
				ALIEN alien[TOTAL_NUM_ALIENS];
                int aliensMax = TOTAL_NUM_ALIENS;
                if (NUM_ALIENS_LEVEL < aliensMax) {
                    aliensMax = NUM_ALIENS_LEVEL;
                }
				int numAliens = 0;
				for (int i = 0; i < aliensMax ; i++) {

					ALIEN newAlien = {ALIEN_START_ROW, ALIEN_START_COL, ALIEN_START_ROW, ALIEN_START_COL,
									   ALIEN_ROW_DELTA, ALIEN_COL_DELTA, ALIEN_WIDTH, ALIEN_HEIGHT,
									   ALIEN_EXIST, ALIEN_WILL_EXIST, TYPE_B, ALIEN_MOVE_COUNTER_START,
                                       ALIEN_RAND_NUM, i, ALIEN_IMG_TYPEB};

					alien[i] = newAlien;
                    numAliens++;
				}

				
                // Game Loop
				while (1) {

					// EVALUATE NEXT POSITIONS
					calculateShipPosition(&ship);
					calculateNewShot(&shot[0], &numShots, &ship, shotWasPressed);
					calculateShotPositions(&shot[0], &numShots);
					calculateAlienPositions(&alien[0], numAliens, ship.col);


					// VERTICAL BLANK
					shotWasPressed = KEY_DOWN_NOW(BUTTON_UP);
					waitForVblank();


					// ERASE	
					eraseOldShip(&ship);
                    eraseOldShots(&shot[0], numShots);
                    eraseOldAliens(&alien[0], &numAliens);

                    // CHECKING existence of shots
                    shouldShotsExist(&shot[0], &numShots);

                    // CHECKING Collisions between Ship and Aliens
                    if (checkShipAlienCollisions(&ship, &alien[0], &numAliens)) {
                        respawnCounter = 0;
                        status.lives -= 1;
                        updateLives(status.lives);
                        if (status.lives <= 0) {
                            state = GAME_OVER;
                            break;
                        }
                    }
                    
                    // Checking Collisions between aliens and shots
                    checkAlienShotCollisions(&alien[0], &numAliens, &shot[0], &numShots, &(status.aliens));

                    // CHECKING Spawns
                    // Checking for respawn of ship
                    if (!ship.exist && respawnCounter < RESPAWN_COUNTER_NUM) {
                        respawnCounter++;
                    } else if (!ship.exist && respawnCounter >= RESPAWN_COUNTER_NUM) {
                        ship.exist = 1;
                        ship.willExist = 1;
                        ship.col = SHIP_START_COL;
                        ship.oldCol = SHIP_START_COL;
                    }

                    // Checking for spawn of new alien
                    // if timer is not done yet
                    if (alienSpawnCounter < ALIEN_SPAWN_COUNT_NUM / status.level) {
                        alienSpawnCounter++;
                    // if timer has completed and there is room for another alien
                    } else if (numAliens < TOTAL_NUM_ALIENS) {
                        
                        ALIEN newAlien = {ALIEN_SPAWN_START_ROW, ALIEN_RAND_COL, ALIEN_SPAWN_START_ROW, ALIEN_START_COL,
                                       ALIEN_ROW_DELTA, ALIEN_COL_DELTA, ALIEN_WIDTH, ALIEN_HEIGHT,
                                       ALIEN_EXIST, ALIEN_WILL_EXIST, TYPE_A, 0, 0, ALIEN_RAND_NUM, ALIEN_IMG_TYPEA};

                        alien[numAliens] = newAlien;
                        
                        // if past level 2, alien has chance to move more quickly
                        if (status.level > 2 && (rand() % 100 < status.level * 10)) {
                            alien[numAliens].rowDelta = 2;
                        }
                        numAliens++;

                        // reset alien spawn counter
                        alienSpawnCounter = 0;

                    }
					

					// DRAW Objects back onto screen
					// Draw shots
					for (int i = 0; i < numShots; i++) {
						if (shot[i].exist) {
							drawRect(shot[i].row, shot[i].col, shot[i].height, shot[i].width, shot[i].color);
							shot[i].oldRow = shot[i].row;
						}
					}

					// Draw ship
					if (ship.exist) {
						drawImage3(ship.row, ship.col, ship.height, ship.width, shipImage);
						//drawRect(ship.row, ship.col, ship.height, ship.width, ship.color);
						ship.oldCol = ship.col;
					}

					// Draw aliens
					for (int i = 0; i < numAliens; i++) {
						if (alien[i].exist) {
                            drawSafeImage3(alien[i].row, alien[i].col, alien[i].height, alien[i].width, alien[i].image);
                            
							alien[i].oldRow = alien[i].row;
							alien[i].oldCol = alien[i].col;
						}
					}


                    // Checking for Special Escape States
                    // User has pressed Select button
					if (KEY_DOWN_NOW(BUTTON_SELECT)) {
						state = START;
						break;
					}

                    // Game Over State
					if (state == GAME_OVER) {
						break;
					}
					
                    // Checking if all aliens are defeated
                    if (numAliens <= 0) {
                        status.level += 1;
                        state = PLAY;
                        break;
                    }
				} // end while

				break;

		} // end switch

        // Checking if Start button was down
		startWasDown = KEY_DOWN_NOW(BUTTON_START);
	}

}

