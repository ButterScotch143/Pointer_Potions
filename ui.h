#ifndef UI_H
#define UI_H

#include "game.h"

// ANSI Escape Codes for Terminal Colors
#define ANSI_RESET   "\x1b[0m"
#define ANSI_RED     "\x1b[31m"
#define ANSI_BLUE    "\x1b[34m"
#define ANSI_YELLOW  "\x1b[33m"
#define ANSI_GREEN   "\x1b[32m"
#define ANSI_MAGENTA "\x1b[35m"

// UI Function Prototypes (UI & Validation Engineer)
void renderBoard(const GameState *game);
void printMessage(const char *msg);
int getValidVialInput(const char *prompt, int max_vials);

#endif // UI_H
