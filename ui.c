
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "ui.h"

#define SCREEN_W 1280
#define SCREEN_H 720

#define VIAL_W 76.0f
#define VIAL_H 190.0f
#define VIAL_Y 190.0f

static const Color BG = {18, 16, 35, 255};
static const Color PANEL = {35, 29, 59, 255};
static const Color PANEL_LIGHT = {48, 39, 75, 255};
static const Color UI_GOLD = {239, 190, 91, 255};
static const Color CREAM = {247, 236, 211, 255};
static const Color MUTED = {174, 163, 194, 255};

static const Color POTION_COLORS[] = {
    {0, 0, 0, 0},
    {235, 76, 91, 255},
    {76, 145, 235, 255},
    {245, 198, 75, 255},
    {83, 190, 126, 255},
    {185, 105, 224, 255},
    {238, 132, 65, 255}
};

/* -------------------------------------------------
   COLOR AND BACKGROUND
------------------------------------------------- */

static Color potionColor(int id)
{
    if (id < 1 || id > 6)
        return LIGHTGRAY;

    return POTION_COLORS[id];
}

static void drawBackground(void)
{
    ClearBackground(BG);

    // Stars
    for (int i = 0; i < 45; i++) {
        int x = (i * 173 + 37) % SCREEN_W;
        int y = (i * 97 + 23) % SCREEN_H;

        DrawCircle(
            x, y,
            (i % 3 == 0) ? 2.0f : 1.0f,
            (Color){120, 101, 157, 90}
        );
    }

    // Top accent
    DrawRectangle(0, 0, SCREEN_W, 6, UI_GOLD);

    // Bottom accent
    DrawRectangle(0, SCREEN_H - 5, SCREEN_W, 5, PANEL);
}

static void drawPanel(Rectangle r)
{
    DrawRectangleRounded(r, 0.06f, 12, PANEL);

    DrawRectangleRoundedLines(
        r, 0.06f, 12,
        (Color){111, 94, 143, 180}
    );
}

/* -------------------------------------------------
   BUTTON
------------------------------------------------- */

static bool button(Rectangle r, const char *label, bool highlighted)
{
    Vector2 mouse = GetMousePosition();

    bool hover = CheckCollisionPointRec(mouse, r);

    Color fill;

    if (highlighted) {
        fill = hover
            ? (Color){255, 215, 125, 255}
            : UI_GOLD;
    } else {
        fill = hover
            ? (Color){87, 70, 119, 255}
            : PANEL_LIGHT;
    }

    DrawRectangleRounded(r, 0.20f, 12, fill);

    DrawRectangleRoundedLines(
        r, 0.20f, 12,
        highlighted ? UI_GOLD : (Color){111, 94, 143, 255}
    );

    int font = 20;
    int width = MeasureText(label, font);

    Color textColor = highlighted ? BG : CREAM;

    if (highlighted && !hover)
        textColor = BG;

    DrawText(
        label,
        (int)(r.x + (r.width - width) / 2),
        (int)(r.y + (r.height - font) / 2),
        font,
        textColor
    );

    return hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

/* -------------------------------------------------
   TITLES
------------------------------------------------- */

static void titleText(const char *subtitle)
{
    const char *title = "POINTER POTIONS";

    int width = MeasureText(title, 48);

    DrawText(
        title,
        (SCREEN_W - width) / 2,
        55,
        48,
        UI_GOLD
    );

    int subWidth = MeasureText(subtitle, 19);

    DrawText(
        subtitle,
        (SCREEN_W - subWidth) / 2,
        115,
        19,
        CREAM
    );
}

/* -------------------------------------------------
   VIAL POSITION
------------------------------------------------- */

static float getVialSpacing(int n)
{
    return (n <= 4) ? 65.0f : 30.0f;
}

static float getVialStartX(int n)
{
    float spacing = getVialSpacing(n);
    float total = n * VIAL_W + (n - 1) * spacing;

    return (SCREEN_W - total) / 2.0f;
}

/* -------------------------------------------------
   VIAL DRAWING
------------------------------------------------- */

static void drawVial(
    const Vial *vial,
    int index,
    float x,
    float y,
    bool selected,
    bool hinted
)
{
    Rectangle glass = {
        x, y, VIAL_W, VIAL_H
    };

    // Hint highlight
    if (hinted) {
        DrawRectangleRounded(
            (Rectangle){x - 7, y - 7, VIAL_W + 14, VIAL_H + 35},
            0.16f, 10,
            (Color){239, 190, 91, 45}
        );
    }

    // Selection highlight
    if (selected) {
        DrawRectangleRounded(
            (Rectangle){x - 8, y - 8, VIAL_W + 16, VIAL_H + 36},
            0.16f, 10,
            (Color){100, 160, 255, 55}
        );
    }

    // Glass body
    DrawRectangleRounded(
        glass,
        0.18f,
        12,
        (Color){210, 225, 255, 24}
    );

    DrawRectangleRoundedLines(
        glass,
        0.18f,
        12,
        selected ? SKYBLUE : (Color){195, 207, 231, 210}
    );

    // Liquid
    for (int slot = 0; slot <= vial->top_idx && slot < 4; slot++) {

        float liquidH = 39.0f;

        float lx = x + 8;
        float ly = y + VIAL_H - 8 - (slot + 1) * liquidH;

        Rectangle liquid = {
            lx,
            ly,
            VIAL_W - 16,
            liquidH - 2
        };

        DrawRectangleRounded(
            liquid,
            0.12f,
            5,
            potionColor(vial->slots[slot])
        );

        // Liquid reflection
        DrawRectangle(
            (int)lx + 4,
            (int)ly + 4,
            5,
            (int)liquidH - 12,
            (Color){255, 255, 255, 45}
        );
    }

    // Bottom glass line
    DrawLine(
        (int)(x + 5),
        (int)(y + VIAL_H - 3),
        (int)(x + VIAL_W - 5),
        (int)(y + VIAL_H - 3),
        (Color){210, 225, 255, 190}
    );

    // Vial label
    char label[24];

    snprintf(label, sizeof(label), "VIAL %d", index + 1);

    int lw = MeasureText(label, 15);

    DrawText(
        label,
        (int)(x + (VIAL_W - lw) / 2),
        (int)(y + VIAL_H + 12),
        15,
        selected ? SKYBLUE : MUTED
    );
}

/* -------------------------------------------------
   MAIN MENU
------------------------------------------------- */

void drawMainMenu(void)
{
    drawBackground();

    titleText("THE ALCHEMY LAB | A LIQUID SORTING PUZZLE");

    DrawText(
        "Sort every potion. Master the art of alchemy.",
        380, 185, 22, MUTED
    );

    // Menu panel
    drawPanel((Rectangle){415, 245, 450, 390});

    float x = 465;
    float y = 280;
    float w = 350;
    float h = 58;
    float gap = 16;

    button((Rectangle){x, y, w, h}, "NEW GAME", true);

    button(
        (Rectangle){x, y + (h + gap), w, h},
        "CONTINUE", false
    );

    button(
        (Rectangle){x, y + 2 * (h + gap), w, h},
        "INSTRUCTIONS", false
    );

    button(
        (Rectangle){x, y + 3 * (h + gap), w, h},
        "EXIT", false
    );

    const char *footer = "A POTION SORTING GAME MADE WITH C AND RAYLIB";

    int fw = MeasureText(footer, 14);

    DrawText(
        footer,
        (SCREEN_W - fw) / 2,
        680,
        14,
        MUTED
    );
}

/* -------------------------------------------------
   INSTRUCTIONS
------------------------------------------------- */

void drawInstructions(void)
{
    drawBackground();

    titleText("HOW TO PLAY");

    drawPanel((Rectangle){190, 175, 900, 415});

    const char *lines[] = {
        "01   Select a vial, then select a destination vial.",
        "02   Pour into an empty vial or onto the same color.",
        "03   Matching top layers move together when possible.",
        "04   Each vial holds up to four liquid units.",
        "05   Sort every color into a full vial to win.",
        "06   Hints suggest moves. Undo reverses your last move.",
        "07   Answer riddles to earn five extra moves."
    };

    for (int i = 0; i < 7; i++) {

        DrawCircle(
            235, 235 + i * 47,
            4, UI_GOLD
        );

        DrawText(
            lines[i],
            255,
            225 + i * 47,
            20,
            CREAM
        );
    }

    button(
        (Rectangle){490, 615, 300, 54},
        "BACK TO MENU",
        true
    );
}

/* -------------------------------------------------
   GAME SCREEN
------------------------------------------------- */

void drawGameScreen(
    const GameState *game,
    int selected_vial,
    int hint_from,
    int hint_to,
    const char *message,
    float animation_progress,
    int anim_from,
    int anim_to
)
{
    drawBackground();

    // Header
    DrawText("POINTER POTIONS", 45, 25, 30, UI_GOLD);

    char header[128];

    snprintf(
        header,
        sizeof(header),
        "LEVEL %d       MOVES: %d       HINTS: %d       POWER-UPS: %d",
        game->level,
        game->moves_remaining,
        game->hints_remaining,
        game->powerups_remaining
    );

    DrawText(header, 45, 75, 18, CREAM);

    DrawLine(
        45, 112,
        1235, 112,
        (Color){111, 94, 143, 180}
    );

    // Game board
    drawPanel((Rectangle){65, 145, 1150, 340});

    int n = game->num_vials;

    float spacing = getVialSpacing(n);
    float startX = getVialStartX(n);

    for (int i = 0; i < n; i++) {

        bool hinted =
            (i == hint_from || i == hint_to) && hint_from >= 0;

        drawVial(
            &game->vials[i],
            i,
            startX + i * (VIAL_W + spacing),
            VIAL_Y,
            i == selected_vial,
            hinted
        );
    }

    // Pour animation
    if (
        animation_progress > 0.0f &&
        anim_from >= 0 &&
        anim_to >= 0 &&
        anim_from < n &&
        anim_to < n
    ) {
        float sx = startX + anim_from * (VIAL_W + spacing) + VIAL_W / 2;
        float tx = startX + anim_to * (VIAL_W + spacing) + VIAL_W / 2;

        float sy = VIAL_Y + 35;
        float ty = VIAL_Y + 35;

        float px = sx + (tx - sx) * animation_progress;

        float py =
            sy + (ty - sy) * animation_progress
            - 65.0f * sinf(animation_progress * 3.14159f);

        DrawCircleV((Vector2){px, py}, 12, UI_GOLD);
        DrawCircleV((Vector2){px, py}, 6, CREAM);
    }

    // Instructions / feedback
    if (message && message[0]) {

        int mw = MeasureText(message, 19);

        DrawText(
            message,
            (SCREEN_W - mw) / 2,
            505,
            19,
            UI_GOLD
        );

    } else {

        const char *instruction =
            "SELECT A SOURCE VIAL, THEN A DESTINATION VIAL";

        int iw = MeasureText(instruction, 18);

        DrawText(
            instruction,
            (SCREEN_W - iw) / 2,
            505,
            18,
            MUTED
        );
    }

    // Control buttons
    float bx = 179;
    float by = 565;
    float bw = 170;
    float bh = 55;
    float gap = 18;

    button((Rectangle){bx, by, bw, bh}, "HINT", false);

    button(
        (Rectangle){bx + (bw + gap), by, bw, bh},
        "UNDO", false
    );

    button(
        (Rectangle){bx + 2 * (bw + gap), by, bw, bh},
        "POWER-UP", true
    );

    button(
        (Rectangle){bx + 3 * (bw + gap), by, bw, bh},
        "RESTART", false
    );

    button(
        (Rectangle){bx + 4 * (bw + gap), by, bw, bh},
        "MENU", false
    );

    DrawText(
        "POINTER POTIONS  |  ALCHEMY LAB",
        45, 675, 14, MUTED
    );
}

/* -------------------------------------------------
   END SCREEN
------------------------------------------------- */

void drawEndScreen(const GameState *game)
{
    drawBackground();

    titleText(
        game->is_won
            ? "POTION MASTERED!"
            : "THE MIXTURE FAILED"
    );

    drawPanel((Rectangle){320, 185, 640, 430});

    const char *line = game->is_won
        ? "Every potion has found its proper color."
        : "You ran out of moves. Try another mixture.";

    int w = MeasureText(line, 21);

    DrawText(
        line,
        (SCREEN_W - w) / 2,
        250,
        21,
        CREAM
    );

    char score[80];

    snprintf(
        score,
        sizeof(score),
        "LEVEL %d       SCORE %d",
        game->level,
        game->score
    );

    int sw = MeasureText(score, 22);

    DrawText(
        score,
        (SCREEN_W - sw) / 2,
        310,
        22,
        UI_GOLD
    );

    if (game->is_won) {

        button(
            (Rectangle){465, 380, 350, 55},
            "NEXT LEVEL",
            true
        );

        button(
            (Rectangle){465, 450, 350, 55},
            "REPLAY LEVEL",
            false
        );

    } else {

        button(
            (Rectangle){465, 410, 350, 55},
            "TRY AGAIN",
            true
        );
    }

    button(
        (Rectangle){465, 535, 350, 55},
        "MAIN MENU",
        false
    );
}

/* -------------------------------------------------
   RIDDLE SCREEN
------------------------------------------------- */

void drawRiddleScreen(
    int selected_answer,
    const char *feedback
)
{
    drawBackground();

    titleText("THE ALCHEMIST'S RIDDLE");

    drawPanel((Rectangle){230, 180, 820, 400});

    DrawText(
        "I have a face and two hands,",
        330, 235, 25, CREAM
    );

    DrawText(
        "but no arms or legs.",
        330, 275, 25, CREAM
    );

    DrawText(
        "WHAT AM I?",
        330, 330, 21, UI_GOLD
    );

    const char *answers[] = {
        "A clock",
        "A potion",
        "A mirror",
        "A book"
    };

    for (int i = 0; i < 4; i++) {

        Rectangle r = {
            330 + (i % 2) * 320.0f,
            375 + (i / 2) * 65.0f,
            280,
            50
        };

        bool hover = CheckCollisionPointRec(
            GetMousePosition(),
            r
        );

        Color fill = BG;

        if (selected_answer == i)
            fill = (Color){105, 82, 135, 255};
        else if (hover)
            fill = PANEL_LIGHT;

        DrawRectangleRounded(r, 0.18f, 10, fill);

        DrawRectangleRoundedLines(
            r, 0.18f, 10,
            (Color){111, 94, 143, 180}
        );

        DrawText(
            TextFormat("%d. %s", i + 1, answers[i]),
            (int)r.x + 20,
            (int)r.y + 15,
            19,
            CREAM
        );
    }

    if (feedback && feedback[0]) {

        int fw = MeasureText(feedback, 19);

        DrawText(
            feedback,
            (SCREEN_W - fw) / 2,
            525,
            19,
            UI_GOLD
        );
    }

    DrawText(
        "ANSWER CORRECTLY TO EARN FIVE EXTRA MOVES.",
        365, 600, 17, MUTED
    );

    button(
        (Rectangle){510, 645, 260, 45},
        "RETURN TO GAME",
        false
    );
}

/* -------------------------------------------------
   VIAL CLICK DETECTION
------------------------------------------------- */

int getClickedVial(const GameState *game)
{
    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        return -1;

    int n = game->num_vials;

    float spacing = getVialSpacing(n);
    float startX = getVialStartX(n);

    for (int i = 0; i < n; i++) {

        Rectangle r = {
            startX + i * (VIAL_W + spacing) - 8,
            VIAL_Y - 8,
            VIAL_W + 16,
            VIAL_H + 36
        };

        if (
            CheckCollisionPointRec(
                GetMousePosition(),
                r
            )
        ) {
            return i;
        }
    }

    return -1;
}

/* -------------------------------------------------
   MENU ACTIONS
------------------------------------------------- */

UIAction getMenuAction(void)
{
    float x = 465;
    float y = 280;
    float w = 350;
    float h = 58;
    float gap = 16;

    if (button((Rectangle){x, y, w, h}, "NEW GAME", true))
        return UI_NEW_GAME;

    if (button((Rectangle){x, y + (h + gap), w, h}, "CONTINUE", false))
        return UI_CONTINUE;

    if (button((Rectangle){x, y + 2 * (h + gap), w, h}, "INSTRUCTIONS", false))
        return UI_INSTRUCTIONS;

    if (button((Rectangle){x, y + 3 * (h + gap), w, h}, "EXIT", false))
        return UI_EXIT;

    return UI_NONE;
}

/* -------------------------------------------------
   GAME ACTIONS
------------------------------------------------- */

UIAction getGameAction(void)
{
    float bx = 179;
    float by = 565;
    float bw = 170;
    float bh = 55;
    float gap = 18;

    if (button((Rectangle){bx, by, bw, bh}, "HINT", false))
        return UI_HINT;

    if (button((Rectangle){bx + (bw + gap), by, bw, bh}, "UNDO", false))
        return UI_UNDO;

    if (button((Rectangle){bx + 2 * (bw + gap), by, bw, bh}, "POWER-UP", true))
        return UI_POWERUP;

    if (button((Rectangle){bx + 3 * (bw + gap), by, bw, bh}, "RESTART", false))
        return UI_RESTART;

    if (button((Rectangle){bx + 4 * (bw + gap), by, bw, bh}, "MENU", false))
        return UI_MENU;

    return UI_NONE;
}

/* -------------------------------------------------
   END SCREEN ACTIONS
------------------------------------------------- */

UIAction getEndAction(bool won)
{
    if (won) {

        if (button((Rectangle){465, 380, 350, 55}, "NEXT LEVEL", true))
            return UI_NEXT_LEVEL;

        if (button((Rectangle){465, 450, 350, 55}, "REPLAY LEVEL", false))
            return UI_RESTART;

    } else {

        if (button((Rectangle){465, 410, 350, 55}, "TRY AGAIN", true))
            return UI_RESTART;
    }

    if (button((Rectangle){465, 535, 350, 55}, "MAIN MENU", false))
        return UI_MENU;

    return UI_NONE;
}

/* -------------------------------------------------
   RIDDLE ACTIONS
------------------------------------------------- */


UIAction getRiddleAction(int *selected_answer)
{
    const Rectangle answers[4] = {
        {330, 375, 280, 50},
        {650, 375, 280, 50},
        {330, 440, 280, 50},
        {650, 440, 280, 50}
    };

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        for (int i = 0; i < 4; i++)
        {
            if (CheckCollisionPointRec(
                GetMousePosition(),
                answers[i]))
            {
                *selected_answer = i;

                return (i == 0)
                    ? UI_CONFIRM_RIDDLE
                    : UI_RIDDLE_WRONG;
            }
        }
    }

    if (button(
        (Rectangle){510, 645, 260, 45},
        "RETURN TO GAME",
        false))
    {
        return UI_SKIP_RIDDLE;
    }

    return UI_NONE;
}

