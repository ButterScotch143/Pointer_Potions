#ifndef UI_H
#define UI_H

#include "raylib.h"
#include "game.h"

typedef enum {
    UI_NONE,
    UI_NEW_GAME,
    UI_CONTINUE,
    UI_INSTRUCTIONS,
    UI_EXIT,
    UI_BACK,
    UI_HINT,
    UI_UNDO,
    UI_POWERUP,
    UI_RESTART,
    UI_MENU,
    UI_NEXT_LEVEL,
    UI_CONFIRM_RIDDLE,
    UI_SKIP_RIDDLE,
    UI_RIDDLE_WRONG
    UI_STORY_NEXT,   // I added these
    UI_STORY_SKIP
} UIAction;

void selectRandomRiddle(void);
void drawMainMenu(void);
void drawInstructions(void);
void drawGameScreen(const GameState *game, int selected_vial,
                    int hint_from, int hint_to, const char *message,
                    float animation_progress, int anim_from, int anim_to);
void drawEndScreen(const GameState *game);
void drawRiddleScreen(int selected_answer, const char *feedback);
int getClickedVial(const GameState *game);
UIAction getMenuAction(void);
UIAction getGameAction(void);
UIAction getEndAction(bool won);
UIAction getRiddleAction(int *selected_answer);

//for the storyline
void initStoryAssets(void);
void unloadStoryAssets(void);
void drawStoryScreen(int slideIndex, int visibleChars);
int getStorySlideCount(void);
int getStorySlideTextLength(int slideIndex);
UIAction getStoryAction(void);

#endif
