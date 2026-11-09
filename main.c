#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "ui.h"
#include "storage.h"

int main(void) {
    GameState game;
    init_game(&game);

    while (!game.is_game_over) {
        renderBoard(&game);

        int from = getValidVialInput("Select source vial (1-6): ", game.num_vials) - 1;
        int to = getValidVialInput("Select destination vial (1-6): ", game.num_vials) - 1;

        if (!makeMove(&game, from, to)) {
            printMessage("Invalid move! Potion colors do not match or target vial is full.");
        } else if (checkWinCondition(&game)) {
            game.is_won = true;
            game.is_game_over = true;
        } else if (game.moves_remaining <= 0) {
            game.is_game_over = true;
        }
    }

    renderBoard(&game);
    if (game.is_won) {
        printMessage("Alchemical Mastery Achieved! You sorted all potions.");
    } else {
        printMessage("Out of moves! The potion mixture exploded.");
    }

    return 0;
}
