
#include <stdio.h>
#include <stdlib.h>
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

static Texture2D titleArt;
static bool titleArtLoaded = false;

static Texture2D cutsceneTex[CUTSCENE_PAGES];
static bool cutsceneLoaded = false;

static const char *cutsceneText[CUTSCENE_PAGES] = {
    "It was a quiet, peaceful night...",
    "Inside a tower, a wizard was busy with their work...",
    "They were experimenting with different magical recipes for their potions.",
    "Everything was going smoothly, until...",
    ""
};

/* -------------------------------------------------
   FOR IMAGE ASSETS & CUTSCENES
------------------------------------------------- */
void unloadTitleArt(void)
{
    if (titleArtLoaded) UnloadTexture(titleArt);
}


/* -------------------------------------------------
   RIDDLES
------------------------------------------------- */

typedef struct {
    const char *question;
    const char *answers[4];
    int correctAnswer;
} Riddle;

static const Riddle riddles[] = {
    {
        "What has hands but cannot clap?",
        {"A clock", "A robot", "A person", "A tree"},
        0
    },
    {
        "What has keys but no locks?",
        {"A door", "A keyboard", "A box", "A car"},
        1
    },
    {
        "What gets wetter the more it dries?",
        {"A towel", "A sponge", "A cloud", "A river"},
        0
    },
    {
        "What has a neck but no head?",
        {"A snake", "A bottle", "A shirt", "A giraffe"},
        1
    },
    {
        "What has many teeth but cannot bite?",
        {"A dog", "A comb", "A shark", "A zipper"},
        1
    },
    {
        "What goes up but never comes down?",
        {"A balloon", "Smoke", "Your age", "An elevator"},
        2
    }, 
    // --- ADD NEW RIDDLES BELOW ---
    {
        "I speak without a mouth and hear without ears. What am I?",
        {"An echo", "A ghost", "The wind", "A shadow"},
        0
    },
    {
        "The more of this there is, the less you see. What is it?",
        {"Fog", "Darkness", "Smoke", "Light"},
        1
    },
    {
        "What has cities, but no houses; forests, but no trees; and water, but no fish?",
        {"A painting", "A dream", "A map", "A globe"},
        2
    },
    {
        "What can you break, even if you never pick it up or touch it?",
        {"Glass", "A promise", "Silence", "A record"},
        1
    },
    {
        "I have branches, but no fruit, trunk, or leaves. What am I?",
        {"A river", "A bank", "A library", "A family tree"},
        1
    }
};

#define RIDDLE_COUNT (sizeof(riddles) / sizeof(riddles[0]))

static int currentRiddle = 0;

void selectRandomRiddle(void)
{
    currentRiddle = rand() % RIDDLE_COUNT;
}

/* -------------------------------------------------
   POTION COLORS
------------------------------------------------- */

static const Color POTION_COLORS[] = {
    {0, 0, 0, 0},
    {235, 76, 91, 255},
    {76, 145, 235, 255},
    {245, 198, 75, 255},
    {83, 190, 126, 255},
    {185, 105, 224, 255},
    {238, 132, 65, 255}
};

static Color potionColor(int id)
{
    if (id < 1 || id > 6)
        return LIGHTGRAY;

    return POTION_COLORS[id];
}

/* -------------------------------------------------
   BACKGROUND
------------------------------------------------- */

static void drawBackground(void)
{
    ClearBackground(BG);

    for (int i = 0; i < 45; i++) {
        int x = (i * 173 + 37) % SCREEN_W;
        int y = (i * 97 + 23) % SCREEN_H;

        DrawCircle(
            x, y,
            (i % 3 == 0) ? 2.0f : 1.0f,
            (Color){120, 101, 157, 90}
        );
    }

    DrawRectangle(0, 0, SCREEN_W, 6, UI_GOLD);
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

    if (hinted) {
        DrawRectangleRounded(
            (Rectangle){x - 7, y - 7, VIAL_W + 14, VIAL_H + 35},
            0.16f, 10,
            (Color){239, 190, 91, 45}
        );
    }

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

    // Liquid layers
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

        Color liquidColor = potionColor(vial->slots[slot]);

        DrawRectangleRounded(
            liquid,
            0.12f,
            8,
            liquidColor
        );

        // Animated liquid surface
        if (slot == vial->top_idx) {

            float waveTime = (float)GetTime() * 3.0f;

            for (int i = 0; i < 20; i++) {

                float px1 = lx + i * 3.0f;
                float px2 = px1 + 3.0f;

                float py1 = ly + sinf(waveTime + i * 0.35f) * 2.0f;
                float py2 = ly + sinf(waveTime + (i + 1) * 0.35f) * 2.0f;

                DrawLineEx(
                    (Vector2){px1, py1},
                    (Vector2){px2, py2},
                    3.0f,
                    Fade(WHITE, 0.45f)
                );
            }

            // Bubbles
            DrawCircleV(
                (Vector2){
                    lx + 20 + sinf(waveTime) * 5,
                    ly + 22 + sinf(waveTime * 1.5f) * 5
                },
                2.5f,
                Fade(WHITE, 0.45f)
            );

            DrawCircleV(
                (Vector2){
                    lx + 45 + cosf(waveTime) * 4,
                    ly + 12 + cosf(waveTime * 1.3f) * 4
                },
                2.0f,
                Fade(WHITE, 0.35f)
            );
        }

        // Liquid reflection
        DrawRectangle(
            (int)lx + 4,
            (int)ly + 6,
            5,
            (int)liquidH - 14,
            Fade(WHITE, 0.25f)
        );
    }

    // Glass outline
    DrawRectangleRoundedLines(
        glass,
        0.18f,
        12,
        selected ? SKYBLUE : (Color){195, 207, 231, 210}
    );

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

    if (!titleArtLoaded)
    {
        titleArt = LoadTexture("DrawnAssets/TitleArt.png");
        SetTextureFilter(titleArt, TEXTURE_FILTER_BILINEAR);
        titleArtLoaded = true;
    }

    float scale = 0.7f;
    float artW = titleArt.width * scale;
    DrawTextureEx(
        titleArt,
        (Vector2){ (SCREEN_W - artW) / 2.0f, 12 },
        0.0f, scale, WHITE
    );

    const char *sub = "THE ALCHEMY LAB | A LIQUID SORTING PUZZLE";
    int subW = MeasureText(sub, 19);
    DrawText(sub, (SCREEN_W - subW) / 2, 190, 19, CREAM);

    DrawText(
        "Sort every potion. Master the art of alchemy!",
        380, 215, 22, MUTED
    );

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
   LIQUID POURING ANIMATION
------------------------------------------------- */

static void drawLiquidStream(
    float sx,
    float sy,
    float tx,
    float ty,
    Color liquidColor,
    float progress
)
{
    float time = (float)GetTime();

    float cx = (sx + tx) / 2.0f;
    float cy = sy - 75.0f;

    Vector2 previous = {sx, sy};

    // Curved liquid stream
    for (int i = 1; i <= 35; i++) {

        float t = (float)i / 35.0f;

        if (t > progress)
            break;

        float inv = 1.0f - t;

        float wave = sinf(time * 12.0f + t * 18.0f) * 2.5f;

        Vector2 current = {
            inv * inv * sx + 2 * inv * t * cx + t * t * tx + wave,
            inv * inv * sy + 2 * inv * t * cy + t * t * ty
        };

        float thickness =
            8.0f + sinf(t * 20.0f + time * 8.0f) * 1.5f;

        DrawLineEx(
            previous,
            current,
            thickness,
            liquidColor
        );

        DrawLineEx(
            previous,
            current,
            2.0f,
            Fade(WHITE, 0.45f)
        );

        previous = current;
    }

    // Moving droplets
    for (int i = 0; i < 12; i++) {

        float t = fmodf(
            time * 1.4f + i * 0.085f,
            1.0f
        );

        if (t > progress)
            continue;

        float inv = 1.0f - t;

        float wave = sinf(time * 10.0f + i) * 3.0f;

        Vector2 drop = {
            inv * inv * sx + 2 * inv * t * cx + t * t * tx + wave,
            inv * inv * sy + 2 * inv * t * cy + t * t * ty
        };

        float radius = 2.0f + (i % 3);

        DrawCircleV(drop, radius, liquidColor);
        DrawCircleV(drop, radius * 0.4f, Fade(WHITE, 0.65f));
    }

    // Splash effect
    if (progress > 0.75f) {

        float splashProgress = (progress - 0.75f) / 0.25f;

        for (int i = 0; i < 8; i++) {

            float angle = i * 0.785398f;

            float distance = splashProgress * 17.0f;

            Vector2 splash = {
                tx + cosf(angle) * distance,
                ty + sinf(angle) * distance
            };

            DrawCircleV(
                splash,
                2.5f * (1.0f - splashProgress * 0.4f),
                liquidColor
            );
        }
    }
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

    // Draw all vials
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

    // Realistic liquid pouring
    if (
        animation_progress > 0.0f &&
        anim_from >= 0 &&
        anim_to >= 0 &&
        anim_from < n &&
        anim_to < n
    )
    {
        float sx = startX +
            anim_from * (VIAL_W + spacing) + VIAL_W / 2;

        float tx = startX +
            anim_to * (VIAL_W + spacing) + VIAL_W / 2;

        float direction = (tx > sx) ? 1.0f : -1.0f;

        int top = game->vials[anim_from].top_idx;

        if (top >= 0 && top < VIAL_CAPACITY) {

            Color liquidColor = potionColor(
                game->vials[anim_from].slots[top]
            );

            float sourceX = sx + direction * 20.0f;
            float sourceY = VIAL_Y + 8.0f;

            float targetX = tx - direction * 5.0f;
            float targetY = VIAL_Y + 12.0f;

            drawLiquidStream(
                sourceX,
                sourceY,
                targetX,
                targetY,
                liquidColor,
                animation_progress
            );
        }
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

    bool canUsePowerup = (game->powerups_remaining > 0);
   button(
    (Rectangle){bx + 2 * (bw + gap), by, bw, bh},
    canUsePowerup ? "POWER-UP" : "LOCKED",
    canUsePowerup // Un-highlights the gold button when 0 left
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

    const Riddle *riddle = &riddles[currentRiddle];

    DrawText(
        riddle->question,
        300, 290, 25, WHITE
    );

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
            TextFormat(
                "%d. %s",
                i + 1,
                riddle->answers[i]
            ),
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
   INTRODUCTION CUTSCENE
------------------------------------------------- */
UIAction drawCutscene(int page)
{
    if (!cutsceneLoaded)
    {
        for (int i = 0; i < CUTSCENE_PAGES; i++)
        {
            char path[128];
            sprintf(path, "DrawnAssets/introCutscene/introCutscene_%02d.png", i + 1);
            cutsceneTex[i] = LoadTexture(path);
            SetTextureFilter(cutsceneTex[i], TEXTURE_FILTER_BILINEAR);  // smoother when shrunk
        }
        cutsceneLoaded = true;
    }

    ClearBackground(BG);

    float scale = 0.9f;
    float imgW  = cutsceneTex[page].width * scale;
    float imgX  = (SCREEN_W - imgW) / 2.0f;
    float imgY  = 10.0f;
    DrawTextureEx(cutsceneTex[page], (Vector2){ imgX, imgY }, 0.0f, scale, WHITE);

    const char *text = cutsceneText[page];
    if (text[0] != '\0')
    {
        int fs = 26;
        int tw = MeasureText(text, fs);
        DrawText(text, (SCREEN_W - tw) / 2, 595, fs, CREAM);
    }

    const char *counter = TextFormat("%d / %d", page + 1, CUTSCENE_PAGES);
    DrawText(counter, (SCREEN_W - MeasureText(counter, 14)) / 2, 703, 14, MUTED);

    UIAction result = UI_NONE;

    if (page > 0 && button((Rectangle){40, 655, 140, 40}, "BACK", false))
        result = UI_CUTSCENE_PREV;

    if (button((Rectangle){1100, 25, 140, 40}, "SKIP", false))
        result = UI_CUTSCENE_SKIP;

    const char *nextLabel = (page == CUTSCENE_PAGES - 1) ? "BEGIN" : "NEXT";
    if (button((Rectangle){1100, 655, 140, 40}, nextLabel, true))
        result = UI_CUTSCENE_NEXT;

    // Keyboard shortcuts (ESC is deliberately not used: raylib closes the window on it)
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER))
        result = UI_CUTSCENE_NEXT;
    if (page > 0 && IsKeyPressed(KEY_LEFT))
        result = UI_CUTSCENE_PREV;

    return result;
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

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {

        for (int i = 0; i < 4; i++) {

            if (CheckCollisionPointRec(
                GetMousePosition(),
                answers[i]
            )) {

                *selected_answer = i;

                return (i == riddles[currentRiddle].correctAnswer)
                    ? UI_CONFIRM_RIDDLE
                    : UI_RIDDLE_WRONG;
            }
        }
    }

    if (button(
        (Rectangle){510, 645, 260, 45},
        "RETURN TO GAME",
        false
    )) {
        return UI_SKIP_RIDDLE;
    }

    return UI_NONE;
}
