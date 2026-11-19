#ifndef STORAGE_H
#define STORAGE_H

#include <stdbool.h>
#include "game.h"

bool saveGameState(const GameState *game, const char *filename);
bool loadGameState(GameState *game, const char *filename);
void recordHighScore(const char *player_name, int score);

#endif
