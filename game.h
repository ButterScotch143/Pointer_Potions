
#ifndef GAME_H
#define GAME_H

#include <stdbool.h>

/* GAME LIMITS */
#define MAX_VIALS 8
#define VIAL_CAPACITY 4
#define MAX_LEVELS 5
#define MAX_HINTS 3
#define MAX_POWERUPS 3

/* VIAL STRUCTURE */
typedef struct {
    int slots[VIAL_CAPACITY];
    int top_idx;
} Vial;

/* GAME STATE */
typedef struct {
    Vial vials[MAX_VIALS];

    int num_vials;
    int num_colors;

    int level;
    int moves_remaining;

    int hints_remaining;
    int powerups_remaining;

    int score;

    bool is_game_over;
    bool is_won;
} GameState;

/* UNDO SYSTEM */

typedef struct {
    Vial vials[MAX_VIALS];

    int moves_remaining;
    int score;

    bool is_game_over;
    bool is_won;
    bool valid;
} UndoState;


/* GAME FUNCTIONS */
void init_game(GameState *game, int level);

bool isValidMove(
    const GameState *game,
    int from_vial,
    int to_vial
);

bool makeMove(
    GameState *game,
    int from_vial,
    int to_vial
);

bool checkWinCondition(const GameState *game);

bool getHint(
    const GameState *game,
    int *from_vial,
    int *to_vial
);

/* UNDO FUNCTIONS */
void saveUndoState(
    const GameState *game,
    UndoState *undo
);

bool undoMove(
    GameState *game,
    UndoState *undo
);

/* POWER-UP */
void grantPowerup(GameState *game);

#endif
