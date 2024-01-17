/* C MILNE (cmi85), C CHEN (cch217) */

#include "tinygl.h"
#include "pacer.h"
#include <stdlib.h>
#include <stdio.h>
#include "system.h"
#include "button.h"
#include "navswitch.h"
#include "ir_uart.h"
#include "../fonts/font5x7_1.h"
#include "block.h"
#include "led.h"


#define MESSAGE_RATE 15
#define MAIN_LOOP_RATE 300
#define GAME_RATE 1

enum {
    SIGNAL = 'S',           // Signal to make blocks fall faster
    SEND_WIN = 'W',         // Signal that the player has won
    REQUEST_CONNECTION = 'R' // Request for connection
};

// Define game status
typedef enum play_status_t
{
    PLAYER_WIN,           // Player wins the game
    PLAYER_LOSS,          // Player loses the game
    CONNECTING,   // Waiting for a connection
    RUNNING,       // Game is in progress
    RESTART           // Restart the game
} play_status;


// Function to handle user input via the navswitch
void NavswitchFunction(Block* activeBlock, uint8_t* map)
{
    navswitch_update();
    if (navswitch_push_event_p(NAVSWITCH_PUSH) && activeBlock) {
        rotateBlock(activeBlock, map, 1); // Rotateactive blocks when navswitch is pushed
    }
    else if (navswitch_push_event_p(NAVSWITCH_SOUTH) && activeBlock) {
        dropBlock(activeBlock, map); // Make active blocks fall immediately when SOUTH direction of nav is pushed
    }
    else if (navswitch_push_event_p(NAVSWITCH_EAST) && activeBlock) {
        moveBlock(activeBlock, map, tinygl_point(1, 0)); // Move active blocks one column to right when EAST direction of nav is pushed
    }
    else if (navswitch_push_event_p(NAVSWITCH_WEST) && activeBlock) {
        moveBlock(activeBlock, map, tinygl_point(-1, 0)); // Move active blocks one column to left when WEST direction of nav is pushed
    }
}

// Function to check the left button for rotation of the active block
void checkButtonForRotation(Block* activeBlock, uint8_t* map)
{
    button_update();
    if (button_push_event_p(BUTTON1) && activeBlock) {
        rotateBlock(activeBlock, map, 0); // Check the left button for a press and rotate the tetromino
    }
    button_update();
}

// Send connection requests via IR
void sendConnect(void)
{
    int count = 0;
    while (count < 4) {
        ir_uart_putc(REQUEST_CONNECTION); // Send 4 requests to make sure
        count += 1;
    }
}

// Send a signal to make blocks fall faster via IR
void sendSpeed(int nLines)
{
    int line = 0;
    while (line < nLines) {
        ir_uart_putc(SIGNAL); 
        line += 1;
    }
}

// Check if the connection requests has been received via IR
bool receiveConnect(void)
{
    char received = 0;
    bool isConnected = false;
    if (ir_uart_read_ready_p()) {
        received = ir_uart_getc();
        if (received == REQUEST_CONNECTION) {
            isConnected = true;
        }
    }
    return isConnected;
}

// Main game loop
int main(void)
{
    // Initialize all modules
    
    pacer_init(MAIN_LOOP_RATE);
    tinygl_font_set(&font5x7_1);
    tinygl_init(MAIN_LOOP_RATE);
    tinygl_text_speed_set(MESSAGE_RATE);
    tinygl_text_mode_set(TINYGL_TEXT_MODE_SCROLL);
    tinygl_text_speed_set(15); //initial text speed is 15
    system_init();
    led_init();
    ir_uart_init();
    button_init();
    navswitch_init();

    // Initialise grid map
    uint8_t map[7] = { 0 };

    // Define different block shapes in allBlocks array
    Block allBlocks[] = {
        {{0, 0}, 2, {{0, 1}, {0, 0}}},                 // Small I piece
        {{0, 0}, 3, {{-1, 0}, {0, 0}, {1, 0}}},        // I piece 
        {{0, 0}, 3, {{0, 0}, {1, 0}, {0, 1}}},         // Small L piece
        {{0, 0}, 4, {{-1, 0}, {-1, 1}, {0, 0}, {0, 1}}}, // Square piece 
        {{0, 0}, 4, {{-1, 0}, {0, 1}, {0, 0}, {1, 0}}}, // T piece 
    };

    // Create new block
    Block* activeBlock;
    generateRandomBlock(&activeBlock, allBlocks);

    int isDisplayingMessage = false; // track if should display messages in current frame
    int nLinesCleared = 0; // number of lines cleared in current frame
    int ledFrameCounter = 0; // counts of ticks since last LED changes
    int isLedOn = 0; // current state of LED
    int ledFlashRate = 4; // LED flash rate
    int frameCounter = 0; // count of ticks since the game was last updated
    int player_Score = 0; // number of player's wining rounds
    int speed = 500; // initial speed of the game
    int speedChange = 0; //a variable that helps to change speed

    play_status playStatus = CONNECTING; // Set the initial game status to wait for a connection

    while (1) {
        
        while (playStatus == CONNECTING) 
        {
            pacer_wait();
            tinygl_update();
            // Request IR connections
            if (isDisplayingMessage == false) {
                isDisplayingMessage = true;
                tinygl_text("-HIT NAV-");// Title of game start page
            }
            navswitch_update();
            if (navswitch_push_event_p(NAVSWITCH_PUSH)) {
                sendConnect();  // Send connection requests when push event occurs
                playStatus = RUNNING;
            }
            if (receiveConnect()) {
                playStatus = RUNNING;
            }
            ledFrameCounter += 1;
            if (ledFrameCounter > MAIN_LOOP_RATE / ledFlashRate) {
            ledFrameCounter = 0;
            isLedOn = !isLedOn;
            }
            led_set(LED1, isLedOn);
        }

        while (playStatus == RUNNING) 
        {
            isLedOn = 0;
            led_set(LED1, isLedOn);
            pacer_wait();
            tinygl_update();
            if (activeBlock == NULL) {
                generateRandomBlock(&activeBlock, allBlocks);  // Generate a new random block if there is no active one
            }
            if (ir_uart_read_ready_p()) {
                char received = ir_uart_getc();
                if (received == SEND_WIN) {
                    playStatus = PLAYER_WIN; // Transition to PLAYER_WIN state if "WIN" signal is received
                }
                else if (received == SIGNAL && speed > 0) {
                    speed -= (30 - speedChange); //decrease one player's "speed" value to incease his block fall speed when the other player completed a row
                    speedChange += 2; // To make blocks not fall too fast
                }
            }
            NavswitchFunction(activeBlock, map); 
            checkButtonForRotation(activeBlock, map); 
            frameCounter += 1;
            if (frameCounter > speed / GAME_RATE) { // when "speed" decreases, value of (speed / GAME_RATE) decreases to make blocks fall faster
                frameCounter = 0;
                if (hasBlockLanded(activeBlock, map)) {
                    if (isGameFinished(activeBlock)) {
                        playStatus = PLAYER_LOSS; // Transition to PLAYER_LOSS state if gameplay is finished
                    }
                    convertBlockToMap(activeBlock, map); // Convert the active blocks to game grid
                    activeBlock = NULL;
                }
                else {
                    moveBlock(activeBlock, map, tinygl_point(0, 1));
                }
            }
            tinygl_clear();
            placeLandedBlocks(map);
            nLinesCleared = clearFullRows(map); // Display blocks that have landed on the grid
            sendSpeed(nLinesCleared); // Send a speed signal to the other board when one player completes a row
            if (activeBlock) {
                placeBlock(activeBlock); // Display active blocks
            }
        }

        while (playStatus == PLAYER_LOSS) 
        {
            ledFlashRate = 2;
            tinygl_text_speed_set(20); // Resets the text speed to 20
            tinygl_update();
            char buff1[45];
            sprintf(buff1, "You lose. Wins:%d", player_Score);
            tinygl_text(buff1);
            ir_uart_putc(SEND_WIN);
            playStatus = RESTART; // Transition to RESTART state
        }
        while (playStatus == RESTART) 
        {
            startGame(&activeBlock, map); // Restarts game to initial conditions
            speed = 300;
            playStatus = CONNECTING; // Transition to waiting for connection state
        }

        while (playStatus == PLAYER_WIN) 
        {
            ledFlashRate = 6;
            tinygl_text_speed_set(20); // Resets the text speed to 20
            tinygl_update();
            player_Score += 1;
            char buff2[40];
            sprintf(buff2, "You win! Wins:%d", player_Score);
            tinygl_text(buff2);
            playStatus = RESTART; // Transition to RESTART state
        }

        
    }
}
