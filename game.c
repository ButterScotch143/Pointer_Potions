#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "raylib.h"
#include "game.h"
#include "ui.h"
#include "storage.h"

typedef enum {
    SCREEN_MENU,
    SCREEN_INSTRUCTIONS,
    SCREEN_GAME,
    SCREEN_END,
    SCREEN_RIDDLE
} ScreenState;

int main(void)
{
    const int screenWidth = 1280;
    const int screenHeight = 720;

    const char *saveFile = "pointer_potions.save";

    srand(time(NULL));

    InitWindow(screenWidth, screenHeight, "Pointer Potions: Alchemy Lab");
    SetTargetFPS(60);

    GameState game;
    UndoState undo = {0};

    ScreenState screen = SCREEN_MENU;

    int selectedVial = -1;
    int hintFrom = -1;
    int hintTo = -1;

    int animFrom = -1;
    int animTo = -1;

    int selectedAnswer = -1;
    int riddleGuessesLeft = 2;

    float animTimer = 0.0f;
    const float animDuration = 0.42f;

    char message[128] = "Welcome to the Alchemy Lab!";
    char riddleFeedback[96] = "";

    init_game(&game, 1);

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        // Pour animation
        if (screen == SCREEN_GAME && animTimer > 0)
        {
            animTimer -= dt;

            if (animTimer <= 0)
            {
                animTimer = 0;

                if (makeMove(&game, animFrom, animTo))
                {
                    strcpy(message, "Potion transferred!");

                    saveGameState(&game, saveFile);

                    if (game.is_game_over)
                    {
                        screen = SCREEN_END;
                    }
                }

                selectedVial = -1;
                hintFrom = -1;
                hintTo = -1;

                animFrom = -1;
                animTo = -1;
            }
        }

        BeginDrawing();

        // Main menu
        if (screen == SCREEN_MENU)
        {
            drawMainMenu();

            UIAction action = getMenuAction();

            if (action == UI_NEW_GAME)
            {
                init_game(&game, 1);
                memset(&undo, 0, sizeof(undo));

                selectedVial = -1;
                hintFrom = -1;
                hintTo = -1;

                strcpy(message, "Select a source vial, then a destination.");

                screen = SCREEN_GAME;
            }
            else if (action == UI_CONTINUE)
            {
                if (loadGameState(&game, saveFile))
                {
                    memset(&undo, 0, sizeof(undo));

                    selectedVial = -1;
                    hintFrom = -1;
                    hintTo = -1;

                    strcpy(message, "Saved game loaded.");

                    if (game.is_game_over)
                        screen = SCREEN_END;
                    else
                        screen = SCREEN_GAME;
                }
                else
                {
                    strcpy(message, "No saved game found. Start a new game.");
                }
            }
            else if (action == UI_INSTRUCTIONS)
            {
                screen = SCREEN_INSTRUCTIONS;
            }
            else if (action == UI_EXIT)
            {
                break;
            }
        }

        // Instructions
        else if (screen == SCREEN_INSTRUCTIONS)
        {
            drawInstructions();

            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                Rectangle backButton = {490, 615, 300, 54};

                if (CheckCollisionPointRec(GetMousePosition(), backButton))
                {
                    screen = SCREEN_MENU;
                }
            }
        }

        // Game screen
        else if (screen == SCREEN_GAME)
        {
            float animationProgress = 0;

            if (animTimer > 0)
            {
                animationProgress = 1.0f - animTimer / animDuration;
            }

            drawGameScreen(
                &game,
                selectedVial,
                hintFrom,
                hintTo,
                message,
                animationProgress,
                animFrom,
                animTo
            );

            // Do not accept another move while pouring
            if (animTimer <= 0)
            {
                UIAction action = getGameAction();

                bool actionHandled = false;

                // Hint
                if (action == UI_HINT)
                {
                    actionHandled = true;

                    if (game.hints_remaining > 0 &&
                        getHint(&game, &hintFrom, &hintTo))
                    {
                        game.hints_remaining--;

                        snprintf(
                            message,
                            sizeof(message),
                            "Hint: pour vial %d into vial %d.",
                            hintFrom + 1,
                            hintTo + 1
                        );
                    }
                    else
                    {
                        strcpy(message, "No hints or valid moves available.");
                    }
                }

                // Undo
                else if (action == UI_UNDO)
                {
                    actionHandled = true;

                    if (undoMove(&game, &undo))
                    {
                        strcpy(message, "Last move reversed.");

                        selectedVial = -1;
                        hintFrom = -1;
                        hintTo = -1;

                        saveGameState(&game, saveFile);
                    }
                    else
                    {
                        strcpy(message, "There is no move to undo.");
                    }
                }

                // Power-up
                else if (action == UI_POWERUP)
                {
                    actionHandled = true;

                        if (game.powerups_remaining > 0)
                    {
                    selectedAnswer = -1;
                    riddleGuessesLeft = 2; // Reset to 2 chances per riddle
                    riddleFeedback[0] = '\0';

                    selectRandomRiddle();
                    screen = SCREEN_RIDDLE;
                    }
                    else
                    {
        strcpy(message, "No more power-ups remaining in this level");
                        }
                    }

                // Restart
                else if (action == UI_RESTART)
                {
                    actionHandled = true;

                    int level = game.level;

                    init_game(&game, level);
                    memset(&undo, 0, sizeof(undo));

                    selectedVial = -1;
                    hintFrom = -1;
                    hintTo = -1;

                    strcpy(message, "Level restarted.");

                    saveGameState(&game, saveFile);
                }

                // Return to menu
                else if (action == UI_MENU)
                {
                    actionHandled = true;

                    saveGameState(&game, saveFile);

                    selectedVial = -1;
                    hintFrom = -1;
                    hintTo = -1;

                    screen = SCREEN_MENU;
                }

                // Select vials
                if (!actionHandled && screen == SCREEN_GAME)
                {
                    int clicked = getClickedVial(&game);

                    if (clicked >= 0)
                    {
                        hintFrom = -1;
                        hintTo = -1;

                        if (selectedVial == -1)
                        {
                            if (game.vials[clicked].top_idx >= 0)
                            {
                                selectedVial = clicked;

                                snprintf(
                                    message,
                                    sizeof(message),
                                    "Vial %d selected.",
                                    clicked + 1
                                );
                            }
                            else
                            {
                                strcpy(message, "Choose a vial containing potion first.");
                            }
                        }
                        else if (selectedVial == clicked)
                        {
                            selectedVial = -1;
                            strcpy(message, "Selection cleared.");
                        }
                        else if (isValidMove(&game, selectedVial, clicked))
                        {
                            saveUndoState(&game, &undo);

                            animFrom = selectedVial;
                            animTo = clicked;

                            animTimer = animDuration;

                            strcpy(message, "Pouring potion...");
                        }
                        else
                        {
                            selectedVial = clicked;

                            snprintf(
                                message,
                                sizeof(message),
                                "Invalid target. Vial %d selected instead.",
                                clicked + 1
                            );
                        }
                    }
                }
            }
        }

        // Riddle screen
        else if (screen == SCREEN_RIDDLE)
        {
            drawRiddleScreen(selectedAnswer, riddleFeedback);

            UIAction action = getRiddleAction(&selectedAnswer);

            if (action == UI_CONFIRM_RIDDLE)
            {
                grantPowerup(&game);

                strcpy(riddleFeedback, "Correct! You earned five extra moves.");
                strcpy(message, "Alchemy riddle solved: +5 moves!");

                selectedVial = -1;

                saveGameState(&game, saveFile);

                screen = SCREEN_GAME;
            }
            else if (action == UI_RIDDLE_WRONG)
            {
            riddleGuessesLeft--;

                if (riddleGuessesLeft > 0)
                {
                    snprintf(
                riddleFeedback,
                    sizeof(riddleFeedback),
                    "Incorrect! You have %d chance left.",
                    riddleGuessesLeft
                        );
                    }
                    else
                        {
                // Failed both chances: consume 1 power-up attempt and return to game
                game.powerups_remaining--;
                    saveGameState(&game, saveFile);

                    snprintf(
                    message,
                    sizeof(message),
                    "Riddle failed! No power-up earned (%d attempts left).",
                    game.powerups_remaining
                        );

                    selectedAnswer = -1;
                    screen = SCREEN_GAME;
                            }
           }
            else if (action == UI_SKIP_RIDDLE)
                {
                game.powerups_remaining--;
                saveGameState(&game, saveFile);

                snprintf(
                    message,
                    sizeof(message),
                    "Riddle abandoned! (%d attempts left)",
                    game.powerups_remaining
                );

    selectedAnswer = -1;
    screen = SCREEN_GAME;
}
        }

        // End screen
        else if (screen == SCREEN_END)
        {
            drawEndScreen(&game);

            UIAction action = getEndAction(game.is_won);

            if (action == UI_RESTART)
            {
                int level = game.level;

                init_game(&game, level);
                memset(&undo, 0, sizeof(undo));

                selectedVial = -1;
                hintFrom = -1;
                hintTo = -1;

                strcpy(message, "Level restarted.");

                saveGameState(&game, saveFile);

                screen = SCREEN_GAME;
            }
            else if (action == UI_NEXT_LEVEL && game.is_won)
            {
                int nextLevel = game.level + 1;

                if (nextLevel > MAX_LEVELS)
                {
                    nextLevel = 1;
                }

                init_game(&game, nextLevel);
                memset(&undo, 0, sizeof(undo));

                selectedVial = -1;
                hintFrom = -1;
                hintTo = -1;

                strcpy(message, "New level started!");

                saveGameState(&game, saveFile);

                screen = SCREEN_GAME;
            }
            else if (action == UI_MENU)
            {
                saveGameState(&game, saveFile);

                selectedVial = -1;

                screen = SCREEN_MENU;
            }
        }

        EndDrawing();
    }

    // Save before closing
    saveGameState(&game, saveFile);

    CloseWindow();

    return 0;
}
