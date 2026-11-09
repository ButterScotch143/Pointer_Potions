#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "ui.h"

void renderBoard(const GameState *game) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif

    printf("\n=======================================================\n");
    printf("               POINTER POTIONS: ALCHEMY LAB\n");
    printf("        Moves Left: %d  |  Hints Remaining: %d\n", game->moves_remaining, game->hints_remaining);
    printf("=======================================================\n\n");

    for (int layer = VIAL_CAPACITY - 1; layer >= 0; layer--) {
        for (int v = 0; v < game->num_vials; v++) {
            if (layer <= game->vials[v].top_idx) {
                int color = game->vials[v].slots[layer];
                switch (color) {
                    case 1:
                        printf("| " ANSI_RED "RED" ANSI_RESET " |   ");
                    break;
                    case 2: printf("| " ANSI_BLUE "BLU" ANSI_RESET " |   "); break;
                    case 3: printf("| " ANSI_YELLOW "YEL" ANSI_RESET " |   "); break;
                    case 4:
                        printf("| " ANSI_GREEN "GRN" ANSI_RESET " |   ");
                        break;
                    case 5:
                        printf("| " ANSI_MAGENTA "MGN" ANSI_RESET " |    ");
                        break;

                    default:
                        printf("|  %d  |   ", color);
                        break;
                }
            } else {
                printf("|     |   ");
            }
        }
        printf("\n");
    }

    for (int v = 0; v < game->num_vials; v++) {
        printf("+-----+   ");
    }
    printf("\n");

    for (int v = 0; v < game->num_vials; v++) {
        printf(" Vial %d   ", v + 1);
    }
    printf("\n\n");
}

void printMessage(const char *msg) {
    printf(">> %s\n", msg);
}

int getValidVialInput(const char *prompt, int max_vials) {
    int choice = 1;

    printf("%s", prompt);
    if (scanf("%d", &choice) != 1) {
        while (getchar() != '\n');
    }
    return choice;
}
