/* C MILNE (cmi85), C CHEN (cch217) */

#include "block.h"
#include "tinygl.h"

// Make it so the grid is clear when the player starts the game
void startGame(Block** activeBlock, uint8_t* gameGrid) 
{
    *activeBlock = NULL;
    
    int row = 0;
    while (row < BOTTOM) {
        gameGrid[row] = 0x00;
        row++;
    }
}

// Place current block on the grid using light up LED's
void placeBlock(Block* block) 
{
    for (int i = 0; i < block->numOffsets; i++) {
        tinygl_point_t gridPosition = calculateGridPosition(block, i);
        tinygl_draw_point(gridPosition, 1);
    }
}

// Allow player to move block, only when there is no wall in the way and they are allowed
void moveBlock(Block* block, uint8_t* map, tinygl_point_t shift) {
    int validMove = 1;

    for (int i = 0; i < block->numOffsets; i++) {
        tinygl_point_t absPos = calculateGridPosition(block, i);
        tinygl_point_t targetPos = addVectors(absPos, shift);

        if (!isCellWithinBounds(targetPos) || isCellTaken(map, targetPos)) {
            validMove = 0;
            break;
        }
    }

    if (validMove && isBlockWithinBounds(block, map, shift)) {
        block->position = addVectors(block->position, shift);
    }
}

// Check to see if the block is going to be completely inside the grid
int isBlockWithinBounds(Block* block, uint8_t* map, tinygl_point_t shift) 
{
    for (int i = 0; i < block->numOffsets; i++) {
        tinygl_point_t absPos = calculateGridPosition(block, i);
        tinygl_point_t targetPos = addVectors(absPos, shift);
        if (!isCellWithinBounds(targetPos) || isCellTaken(map, targetPos)) {
            return 0;
        }
    }
    return 1;
}

// Rotate the block clockwise
void rotateBlock(Block* block, uint8_t* map, int isClockwise) 
{
    Block rotatedBlock;
    rotatedBlock.position = block->position;
    int isRotationValid = 1;

    int offsetIndex = 0;
    while (offsetIndex < block->numOffsets) {
        tinygl_point_t oldOffset = block->offsets[offsetIndex];
        tinygl_point_t newOffset = isClockwise ? tinygl_point(-oldOffset.y, oldOffset.x) : tinygl_point(oldOffset.y, -oldOffset.x);

        rotatedBlock.offsets[offsetIndex] = newOffset;

        if (isCellTaken(map, calculateGridPosition(&rotatedBlock, offsetIndex))) {
            isRotationValid = 0;
            break;
        }

        offsetIndex++;
    }

    if (isRotationValid) {
        offsetIndex = 0;
        while (offsetIndex < block->numOffsets) {
            block->offsets[offsetIndex] = rotatedBlock.offsets[offsetIndex];
            offsetIndex++;
        }

        offsetIndex = 0;
        while (offsetIndex < block->numOffsets) {
            tinygl_point_t currentPos = calculateGridPosition(block, offsetIndex);
            if (currentPos.x <= LEFT_SIDE) {
                moveBlock(block, map, tinygl_point(1, 0));
                break;
            }
            if (currentPos.x >= RIGHT_SIDE) {
                moveBlock(block, map, tinygl_point(-1, 0));
                break;
            }

            offsetIndex++;
        }
    }
}

// Drops the block straight down
void dropBlock(Block* block, uint8_t* map) 
{
    for (; !hasBlockLanded(block, map);) {
        moveBlock(block, map, tinygl_point(0, 1));
    }
}

// Check to see if one of the blocks has landed above the top of the screen, signalling the end of the game
int isGameFinished(Block* block) 
{
    int isOver = 0;

    for (int offsetIndex = 0; offsetIndex < block->numOffsets; offsetIndex++) {
        tinygl_point_t blockPosition = calculateGridPosition(block, offsetIndex);

        if (blockPosition.y <= (TOP + 1)) {
            isOver = 1;
            break;
        }
    }

    return isOver;
}

// Checks if cell in grid has a block in it already (led is lit up) or not
int isCellTaken(uint8_t* map, tinygl_point_t position) 
{
    int isOccupied = (map[position.y] >> position.x) & 1;
    return isOccupied;
}

// Checks to see if cell is within the bounds of the bottom left and right sides of the grid
int isCellWithinBounds(tinygl_point_t point) 
{
    int withinBounds = point.y < BOTTOM && point.x > LEFT_SIDE && point.x < RIGHT_SIDE;
    return withinBounds;
}

// Adds blocks landed to the map grid
void placeLandedBlocks(uint8_t* map) 
{
    for (int rowIndex = 0; rowIndex < BOTTOM; rowIndex++) {
        uint8_t currentRow = map[rowIndex];
        
        for (int colIndex = 0; colIndex < 5; colIndex++) {
            int isOccupied = (currentRow >> colIndex) & 1;
            tinygl_point_t position = tinygl_point(colIndex, rowIndex);
            tinygl_draw_point(position, isOccupied);
        }
    }
}

// Checks if the block has landed on a occupied cell or bottom
int hasBlockLanded(Block* block, uint8_t* map) 
{
    int offsetIndex = 0;
    while (offsetIndex < block->numOffsets) {
        tinygl_point_t currentPosition = calculateGridPosition(block, offsetIndex);
        tinygl_point_t positionBelow = addVectors(currentPosition, tinygl_point(0, 1));

        if (isCellTaken(map, positionBelow) || positionBelow.y >= BOTTOM) {
            return 1;
        }

        offsetIndex++;
    }
    return 0;
}

// Move all lines above the completed line down by one
void shiftRowsDown(uint8_t* map, int clearedRowIndex) 
{
    int currentRow = clearedRowIndex;
    
    while (currentRow > 0) {
        for (int col = 0; col < RIGHT_SIDE; col++) {
            int cellAbove = (map[currentRow - 1] >> col) & 1;
            if (cellAbove) {
                map[currentRow] |= (1 << col);
            } else {
                map[currentRow] &= ~(1 << col);
            }
        }
        currentRow--;
    }
    
    // Clear the top row
    map[0] = 0x00;
}

// Clear row when the lines cells are full
int clearFullRows(uint8_t* map) 
{
    int linesCleared = 0;
    int currentRow = 0;

    while (currentRow < BOTTOM) {
        if (map[currentRow] == 0x1F) {
            linesCleared++;
            map[currentRow] = 0x00;
            shiftRowsDown(map, currentRow);
        } else {
            currentRow++;
        }
    }

    return linesCleared;
}

// Adds the blocks cell location to the grid map
void convertBlockToMap(Block* block, uint8_t* map) 
{
    for (int i = 0; i < block->numOffsets; i++) {
        tinygl_point_t currentPoint = calculateGridPosition(block, i);

        if (isCellWithinBounds(currentPoint)) {
            map[currentPoint.y] |= (1 << currentPoint.x);
        }
    }
}

// Generate random block from the 5 blocks in play.c
void generateRandomBlock(Block** activeBlock, Block* blockArray) 
{
    int randomIndex = rand() % NUM_BLOCKS;
    blockArray[randomIndex].position = tinygl_point(SPAWN);
    *activeBlock = &blockArray[randomIndex];
}

// Gets the position of a block within the grid
tinygl_point_t calculateGridPosition(Block* block, int offsetIndex) 
{
    tinygl_point_t position = block->position;
    tinygl_point_t offset = block->offsets[offsetIndex];
    return addVectors(position, offset);
}

// Simple adding 2 vectors together for changing position of a block by an offset
tinygl_point_t addVectors(tinygl_point_t a, tinygl_point_t b) 
{
    tinygl_point_t result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    return result;
}