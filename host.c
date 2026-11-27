#include "host.h"
#include <string.h>

static Texture2D hostTextures[4];
static bool texturesLoaded = false;

static const char *texturePaths[4] = {
    "assets/wizard_idle.png", //replace these pictures later once we've had in na
    "assets/wizard_happy.png",
    "assets/wizard_worried.png",
    "assets/wizard_happy.png"
};

void InitHost(HostActor *host, float x, float y) {
    if (!texturesLoaded) {
        for (int i = 0; i < 4; i++) {
            hostTextures[i] = LoadTexture(texturePaths[i]);
        }
        texturesLoaded = true;
    }

    host->mood = HOST_IDLE;
    host->position = (Vector2){x, y};
    host->slideInY = 100.0f;
    host->commentDisplayTimer = 0.0f;
    host->currentComment[0] = '\0';
}

void UpdateHost(HostActor *host, float dt) {
    if (host->slideInY > 0.0f) {
        host->slideInY -= dt * 300.0f;
        if (host->slideInY < 0.0f) host->slideInY = 0.0f;
    }

    if (host->commentDisplayTimer > 0.0f) {
        host->commentDisplayTimer -= dt;
        if (host->commentDisplayTimer <= 0.0f) {
            host->mood = HOST_IDLE;
            host->currentComment[0] = '\0';
        }
    }
}

void DrawHost(const HostActor *host) {
    if (!texturesLoaded) return;

    float drawX = host->position.x;
    float drawY = host->position.y + host->slideInY;

    DrawTexture(hostTextures[host->mood], (int)drawX, (int)drawY, WHITE);

    if (host->commentDisplayTimer > 0.0f && strlen(host->currentComment) > 0) {
        float bubbleX = drawX + 120.0f;
        float bubbleY = drawY - 60.0f;
        int textW = MeasureText(host->currentComment, 20);

        Rectangle bubbleRect = { bubbleX, bubbleY, (float)textW + 30.0f, 50.0f };
        DrawRectangleRounded(bubbleRect, 0.3f, 10, (Color){35, 29, 59, 230});
        DrawRectangleRoundedLines(bubbleRect, 0.3f, 10, (Color){239, 190, 91, 200});
        DrawText(host->currentComment, (int)bubbleX + 15, (int)bubbleY + 15, 20, WHITE);
    }
}

void SetHostReaction(HostActor *host, HostMood mood, const char *comment) {
    host->mood = mood;
    strncpy(host->currentComment, comment, sizeof(host->currentComment) - 1);
    host->currentComment[sizeof(host->currentComment) - 1] = '\0';
    host->commentDisplayTimer = 3.5f;
}
