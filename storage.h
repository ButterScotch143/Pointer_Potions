#ifndef STORAGE_H
#define STORAGE_H

#include "game.h"

// Storage Function Prototypes (Persistence Data Engineer)
bool saveGameState(const GameState *game, const char *filename);
bool loadGameState(GameState *game, const char *filename);
void recordHighScore(const char *player_name, int score);

#endif // STORAGE_H
