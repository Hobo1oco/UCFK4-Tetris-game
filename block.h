/* C MILNE (cmi85), C CHEN (cch217)*/

#ifndef BLOCK_H
#define BLOCK_H

#include <stdlib.h>
#include "tinygl.h"

/* Define number parameters for each of the sides, plus the spawn points*/
#define TOP -1
#define BOTTOM 7
#define LEFT_SIDE -1
#define RIGHT_SIDE 5
#define BLOCK_POSSIBLE_OFFSET 4
#define NUM_BLOCKS 5
#define SPAWN 2, 0

/* Initialize Block struct to store each current block*/
typedef struct Block_t {
    tinygl_point_t position;
    int numOffsets;
    tinygl_point_t offsets[BLOCK_POSSIBLE_OFFSET];
} Block;

/* Initialize all functions that will be used in the block.c and play.c scripts*/
void startGame(Block**, uint8_t*);
void placeBlock(Block*);
void moveBlock(Block*, uint8_t*, tinygl_point_t);
int isBlockWithinBounds(Block*, uint8_t*, tinygl_point_t);
void rotateBlock(Block*, uint8_t*, int);
void dropBlock(Block*, uint8_t*);
int isGameFinished(Block*);
int isCellTaken(uint8_t*, tinygl_point_t);
int isCellWithinBounds(tinygl_point_t);
void placeLandedBlocks(uint8_t*);
int hasBlockLanded(Block*, uint8_t*);
void shiftRowsDown(uint8_t*, int);
int clearFullRows(uint8_t*);
void convertBlockToMap(Block*, uint8_t*);
void generateRandomBlock(Block**, Block*);

/* Initialize functions to handle grid positions in the game*/
tinygl_point_t calculateGridPosition(Block*, int);
tinygl_point_t addVectors(tinygl_point_t, tinygl_point_t);

#endif
