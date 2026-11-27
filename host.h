#ifndef HOST_H
#define HOST_H

#include "raylib.h"

typedef enum {
    HOST_IDLE,
    HOST_HAPPY,
    HOST_WORRIED,
    HOST_VICTORY
} HostMood;

typedef struct {
    HostMood mood;
    char currentComment[96];
    float commentDisplayTimer;
    Vector2 position;
    float slideInY;
} HostActor;

void InitHost(HostActor *host, float x, float y);
void UpdateHost(HostActor *host, float dt);
void DrawHost(const HostActor *host);
void SetHostReaction(HostActor *host, HostMood mood, const char *comment);

#endif
