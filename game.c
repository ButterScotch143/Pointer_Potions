#include <stdio.h>
#include "game.h"

// Initialize the game board with sample tubes and colored potion layers
void init_game(GameState *game) {
    game->num_vials = 6;
    game->moves_remaining = 15;
    game->hints_remaining = 3;
    game->is_game_over = false;
    game->is_won = false;

    // Reset all tubes to empty (-1)
    for (int i = 0; i < MAX_VIALS; i++) {
        game->vials[i].top_idx = -1;
    }

    // --- Tube 1 (Index 0): Filled with Red(1), Blue(2), Red(1), Yellow(3) ---
    game->vials[0].slots[0] = 1; // Bottom layer
    game->vials[0].slots[1] = 2;
    game->vials[0].slots[2] = 1;
    game->vials[0].slots[3] = 3; // Top layer
    game->vials[0].top_idx = 3;  // Tube is full (index 3)

    // --- Tube 2 (Index 1): Filled with Yellow(3), Blue(2), Yellow(3), Blue(2) ---
    game->vials[1].slots[0] = 3;
    game->vials[1].slots[1] = 2;
    game->vials[1].slots[2] = 3;
    game->vials[1].slots[3] = 2;
    game->vials[1].top_idx = 3;

    // --- Tube 3 (Index 2): Filled with Red(1), Yellow(3), Blue(2), Red(1) ---
    game->vials[2].slots[0] = 1;
    game->vials[2].slots[1] = 3;
    game->vials[2].slots[2] = 2;
    game->vials[2].slots[3] = 1;
    game->vials[2].top_idx = 3;

    // Tubes 3, 4, 5 (Indices 3, 4, 5) remain empty (top_idx = -1) to serve as buffer tubes!
}

// Check if pouring from one tube to another is allowed by potion rules
bool isValidMove(const GameState *game, int from_vial, int to_vial) {
    // 1. Cannot pour from/to out-of-bounds tubes or into the same tube
    if (from_vial < 0 || from_vial >= game->num_vials) return false;
    if (to_vial < 0 || to_vial >= game->num_vials) return false;
    if (from_vial == to_vial) return false;

    // 2. Cannot pour from an empty tube
    if (game->vials[from_vial].top_idx == -1) return false;

    // 3. Cannot pour into a full tube
    if (game->vials[to_vial].top_idx >= VIAL_CAPACITY - 1) return false;

    // 4. If destination tube is NOT empty, top colors must match
    if (game->vials[to_vial].top_idx != -1) {
        int source_color = game->vials[from_vial].slots[game->vials[from_vial].top_idx];
        int target_color = game->vials[to_vial].slots[game->vials[to_vial].top_idx];
        if (source_color != target_color) return false;
    }

    return true;
}

// Pour top color unit from source tube into destination tube
bool makeMove(GameState *game, int from_vial, int to_vial) {
    if (!isValidMove(game, from_vial, to_vial)) {
        return false;
    }

    // Pop color unit from source tube
    int top_from = game->vials[from_vial].top_idx;
    int color_to_move = game->vials[from_vial].slots[top_from];
    game->vials[from_vial].top_idx--;

    // Push color unit to destination tube
    game->vials[to_vial].top_idx++;
    int top_to = game->vials[to_vial].top_idx;
    game->vials[to_vial].slots[top_to] = color_to_move;

    game->moves_remaining--;
    return true;
}

// Check if all tubes are either completely empty or hold 4 units of identical color
bool checkWinCondition(const GameState *game) {
    for (int i = 0; i < game->num_vials; i++) {
        int top = game->vials[i].top_idx;

        // Empty tube is fine
        if (top == -1) continue;

        // Partially filled tube means game is not won yet
        if (top != VIAL_CAPACITY - 1) return false;

        // Check if all 4 layers in full tube match in color
        int base_color = game->vials[i].slots[0];
        for (int j = 1; j < VIAL_CAPACITY; j++) {
            if (game->vials[i].slots[j] != base_color) return false;
        }
    }
    return true;
}
