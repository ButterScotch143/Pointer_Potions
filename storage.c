
#include <stdio.h>
#include "storage.h"

/* SAVE GAME */
bool saveGameState(
    const GameState *game,
    const char *filename
)
{
    if (!game || !filename)
        return false;

    FILE *file = fopen(filename, "wb");

    if (!file)
        return false;

    size_t written = fwrite(
        game,
        sizeof(GameState),
        1,
        file
    );

    fclose(file);

    return written == 1;
}

/* VALIDATE SAVE DATA */
static bool validateGame(const GameState *game)
{
    if (!game)
        return false;

    if (game->level < 1 || game->level > MAX_LEVELS)
        return false;

    if (
        game->num_vials < 4 ||
        game->num_vials > MAX_VIALS
    )
        return false;

    if (
        game->num_colors < 1 ||
        game->num_colors > (MAX_VIALS - 2) ||
        game->num_colors * VIAL_CAPACITY >
        game->num_vials * VIAL_CAPACITY
    )
        return false;

    if (game->moves_remaining < 0)
        return false;

    if (
        game->hints_remaining < 0 ||
        game->hints_remaining > MAX_HINTS
    )
        return false;

    if (
        game->powerups_remaining < 0 ||
        game->powerups_remaining > MAX_POWERUPS
    )
        return false;

    if (game->score < 0)
        return false;

    for (int i = 0; i < game->num_vials; i++)
    {
        const Vial *v = &game->vials[i];

        if (
            v->top_idx < -1 ||
            v->top_idx >= VIAL_CAPACITY
        )
            return false;

        for (int j = 0; j <= v->top_idx; j++)
        {
            if (
                v->slots[j] < 1 ||
                v->slots[j] > game->num_colors
            )
                return false;
        }
    }

    return true;
}

/* LOAD GAME */
bool loadGameState(
    GameState *game,
    const char *filename
)
{
    if (!game || !filename)
        return false;

    FILE *file = fopen(filename, "rb");

    if (!file)
        return false;

    GameState temp;

    size_t read_count = fread(
        &temp,
        sizeof(GameState),
        1,
        file
    );

    fclose(file);

    if (read_count != 1)
        return false;

    if (!validateGame(&temp))
        return false;

    *game = temp;

    return true;
}

/* RECORD HIGH SCORE */
void recordHighScore(
    const char *player_name,
    int score
)
{
    if (score < 0)
        return;

    FILE *file = fopen("highscore.txt", "a");

    if (!file)
        return;

    fprintf(
        file,
        "%s %d\n",
        player_name ? player_name : "Player",
        score
    );

    fclose(file);
}
