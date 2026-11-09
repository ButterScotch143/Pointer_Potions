#ifndef GAME_H
#define GAME_H

#include <stdbool.h>

#define MAX_VIALS 6
#define VIAL_CAPACITY 4
#define MAX_MOVES 15
#define MAX_HINTS 3

// Represents color IDs: 0 = Empty, 1 = Red, 2 = Blue, 3 = Yellow, etc.
typedef struct {
    int slots[VIAL_CAPACITY]; // Bottom is index 0, top is index top_idx
    int top_idx;              // -1 if empty, 0 to (VIAL_CAPACITY - 1)
} Vial;

typedef struct {
    Vial vials[MAX_VIALS];
    int num_vials;
    int moves_remaining;
    int hints_remaining;
    bool is_game_over;
    bool is_won;
} GameState;

// Core Logic Function Prototypes (Technical Lead)
void init_game(GameState *game);
bool isValidMove(const GameState *game, int from_vial, int to_vial);
bool makeMove(GameState *game, int from_vial, int to_vial);
bool checkWinCondition(const GameState *game);

#endif // GAME_H
