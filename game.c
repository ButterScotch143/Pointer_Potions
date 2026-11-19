
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "game.h"

static const int LEVEL_COLORS[MAX_LEVELS] = {
    2, 3, 4, 5, 6
};

static const int LEVEL_VIALS[MAX_LEVELS] = {
    4, 5, 6, 7, 8
};

static const int LEVEL_MOVES[MAX_LEVELS] = {
    20, 30, 40, 50, 60
};

/* SHUFFLE */
static void shuffle(int *values, int count)
{
    for (int i = count - 1; i > 0; --i) {
        int j = rand() % (i + 1);

        int temp = values[i];
        values[i] = values[j];
        values[j] = temp;
    }
}

/* INITIALIZE GAME */
void init_game(GameState *game, int level)
{
    if (!game) return;

    if (level < 1) level = 1;
    if (level > MAX_LEVELS) level = MAX_LEVELS;

    memset(game, 0, sizeof(*game));

    game->level = level;
    game->num_colors = LEVEL_COLORS[level - 1];
    game->num_vials = LEVEL_VIALS[level - 1];

    game->moves_remaining = LEVEL_MOVES[level - 1];
    game->hints_remaining = MAX_HINTS;
    game->powerups_remaining = MAX_POWERUPS;

    for (int i = 0; i < MAX_VIALS; i++) {
        game->vials[i].top_idx = -1;
    }

    int liquid[MAX_VIALS * VIAL_CAPACITY];

    int count = game->num_colors * VIAL_CAPACITY;

    for (int i = 0; i < game->num_colors; i++) {
        for (int j = 0; j < VIAL_CAPACITY; j++) {
            liquid[i * VIAL_CAPACITY + j] = i + 1;
        }
    }

    shuffle(liquid, count);

    for (int i = 0; i < count; i++) {
        int vial = i / VIAL_CAPACITY;
        int slot = i % VIAL_CAPACITY;

        game->vials[vial].slots[slot] = liquid[i];
        game->vials[vial].top_idx = slot;
    }
}

/* VALIDATE MOVE */
bool isValidMove(
    const GameState *game,
    int from_vial,
    int to_vial
)
{
    if (!game || game->is_game_over)
        return false;

    if (
        from_vial < 0 ||
        to_vial < 0 ||
        from_vial >= game->num_vials ||
        to_vial >= game->num_vials ||
        from_vial == to_vial
    )
        return false;

    const Vial *from = &game->vials[from_vial];
    const Vial *to = &game->vials[to_vial];

    if (from->top_idx < 0)
        return false;

    if (from->top_idx >= VIAL_CAPACITY)
        return false;

    if (to->top_idx >= VIAL_CAPACITY - 1)
        return false;

    int color = from->slots[from->top_idx];

    if (color < 1 || color > game->num_colors)
        return false;

    if (to->top_idx >= 0) {
        int target_color = to->slots[to->top_idx];

        if (color != target_color)
            return false;
    }

    int group_size = 0;

    for (
        int i = from->top_idx;
        i >= 0 && from->slots[i] == color;
        i--
    ) {
        group_size++;
    }

    int free_space =
        VIAL_CAPACITY - (to->top_idx + 1);

    return group_size <= free_space;
}

/* CHECK VICTORY */
bool checkWinCondition(const GameState *game)
{
    if (!game)
        return false;

    int completed_vials = 0;

    for (int i = 0; i < game->num_vials; i++) {
        const Vial *v = &game->vials[i];

        if (v->top_idx == -1)
            continue;

        if (v->top_idx != VIAL_CAPACITY - 1)
            return false;

        int color = v->slots[0];

        if (color < 1 || color > game->num_colors)
            return false;

        for (int j = 1; j < VIAL_CAPACITY; j++) {
            if (v->slots[j] != color)
                return false;
        }

        completed_vials++;
    }

    return completed_vials == game->num_colors;
}

/* MAKE MOVE */
bool makeMove(
    GameState *game,
    int from_vial,
    int to_vial
)
{
    if (!isValidMove(game, from_vial, to_vial))
        return false;

    Vial *from = &game->vials[from_vial];
    Vial *to = &game->vials[to_vial];

    int color = from->slots[from->top_idx];
    int group_size = 0;

    while (
        from->top_idx - group_size >= 0 &&
        from->slots[from->top_idx - group_size] == color
    ) {
        group_size++;
    }

    for (int i = 0; i < group_size; i++) {
        int value = from->slots[from->top_idx];

        from->top_idx--;

        to->top_idx++;

        to->slots[to->top_idx] = value;
    }

    game->moves_remaining--;

    game->score += 10 * group_size;

    if (checkWinCondition(game)) {
        game->is_won = true;
        game->is_game_over = true;
    }
    else if (game->moves_remaining <= 0) {
        game->moves_remaining = 0;
        game->is_game_over = true;
    }

    return true;
}

/* GET HINT */
bool getHint(
    const GameState *game,
    int *from_vial,
    int *to_vial
)
{
    if (!game || !from_vial || !to_vial)
        return false;

    if (game->is_game_over)
        return false;

    // First preference: combine matching colors.
    for (int from = 0; from < game->num_vials; from++) {

        for (int to = 0; to < game->num_vials; to++) {

            if (from == to)
                continue;

            if (
                game->vials[to].top_idx >= 0 &&
                isValidMove(game, from, to)
            ) {
                *from_vial = from;
                *to_vial = to;

                return true;
            }
        }
    }

    // Second preference: move into an empty vial.
    for (int from = 0; from < game->num_vials; from++) {

        for (int to = 0; to < game->num_vials; to++) {

            if (game->vials[to].top_idx == -1 &&
                isValidMove(game, from, to)) {

                *from_vial = from;
                *to_vial = to;

                return true;
            }
        }
    }

    return false;
}

/* SAVE UNDO STATE */
void saveUndoState(
    const GameState *game,
    UndoState *undo
)
{
    if (!game || !undo)
        return;

    memcpy(
        undo->vials,
        game->vials,
        sizeof(game->vials)
    );

    undo->moves_remaining = game->moves_remaining;
    undo->score = game->score;

    undo->is_game_over = game->is_game_over;
    undo->is_won = game->is_won;

    undo->valid = true;
}

/* UNDO MOVE */
bool undoMove(
    GameState *game,
    UndoState *undo
)
{
    if (!game || !undo || !undo->valid)
        return false;

    memcpy(
        game->vials,
        undo->vials,
        sizeof(game->vials)
    );

    game->moves_remaining = undo->moves_remaining;
    game->score = undo->score;

    game->is_game_over = undo->is_game_over;
    game->is_won = undo->is_won;

    undo->valid = false;

    return true;
}

/* GRANT POWER-UP */
void grantPowerup(GameState *game)
{
    if (!game)
        return;

    if (game->powerups_remaining <= 0)
        return;

    game->powerups_remaining--;

    game->moves_remaining += 5;

    game->score += 25;
}
