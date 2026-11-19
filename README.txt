POINTER POTIONS - RAYLIB VERSION
================================

Requirements
------------
- Windows
- MSYS2 UCRT64
- GCC
- raylib package: mingw-w64-ucrt-x86_64-raylib

Build
-----
Open the MSYS2 UCRT64 terminal in this project folder and run:

gcc main.c game.c ui.c storage.c -o Pointer_Potions.exe -std=c99 -Wall -Wextra -I/ucrt64/include -L/ucrt64/lib -lraylib -lopengl32 -lgdi32 -lwinmm

Run
---
./Pointer_Potions.exe

Controls
--------
- Left mouse click: select a source vial, then a destination vial.
- HINT: displays a valid move.
- UNDO: reverses the most recent successful move.
- POWER-UP: opens the riddle. Correct answer gives five extra moves.
- RESTART: restarts the current level.
- MENU: saves and returns to the main menu.
- ESC or window close: closes the game.

Notes
-----
- The project uses the supplied game structures and modular C files.
- Level setup follows the proposal's color, vial, and move-limit table.
- Potion colors are shuffled at the start of each level.
- The current save file is pointer_potions.save.
- This is a first playable implementation. The liquid animation is a simple
  animated droplet travelling between the selected vials. The liquid stacks
  update when the animation finishes.
- The riddle power-up currently grants five extra moves.
