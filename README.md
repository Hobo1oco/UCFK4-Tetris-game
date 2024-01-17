# ![Tetris Logo](photos/tetris_icon.webp)

# TETRIS

# Overview
```
Welcome to 2-Player Tetris, a game developed for the UCFK4 micro-controller. 
In this game, you and your opponent will battle it out to complete lines by moving differently shaped 
pieces that descend onto the playing field. The goal is to clear lines to prevent your 
field from filling up while causing lines the fall faster and faster for your opponent. 
The last player with a clear field wins!
```

**Developers**: Cameron Milne (cmi85) and Chen Chen (cch217)

**Date Started**: 28th September 2023

## Getting files set up

1. Clone the UCFK4 git repository: This contains all the necessary drivers and utilities to run the game
```
$ git clone https://eng-git.canterbury.ac.nz/rmc84/ence260-ucfk4.git 
```


2. Clone Tetris game: This repository needs to be cloned into the apps directory in the ucfk4 folder structure
```
$ cd ence260-ucfk4/apps
$ git clone https://eng-git.canterbury.ac.nz/ence260-2023/group_203.git
$ cd tetris
```

# How to start game
From within the tetris directory, the following commands can be run

- `make`: Compiles source code and builds object files
- `make program`: Runs `make` and then loads program into UCFK4 flash memory
- `make clean`: Remove old object files from directory

Run `make program` twice to start the game for both players!

# How to play

- `1` **Start The Game**: The game start display shows `'HIT NAV'`, so push the nav switch to start.
- `2` **Move Blocks**: Move the blocks around the screen with the nav switch. Left button and navswitch push rotate the block clockwise. 
Navswitch down slams the block to the ground.
- `2` **Beat Your Opponent**: Place blocks as fast as you can while keeping all spots filled in to speed up your opponent and give
them a hard time. Making lines with not only help you by giving you more room but disadvantage your opponent.
- `3` **End Of Game**: When the top of one of the players screen is reached, the other player will emerge victorious and the game will end. 
Push nav switch again to start a new game.

# Goal
```
Play against your friends and see who is the best tetris player, where both speed and knowledge are extremely important.
Try see how fast you can together make blocks fall for the other before one of you gives out. Good Luck!
```

# ![Win](photos/Untitled.png)