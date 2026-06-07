#ifndef RENDERER_H
#define RENDERER_H

#include "structs.h"
#include "raylib.h"

void draw_aliens(AlienNode* aliens, int frame,
                 Texture2D crab1, Texture2D crab2,
                 Texture2D oct1, Texture2D oct2,
                 Texture2D sq1, Texture2D sq2);

void draw_bunkers(GameState* game, Texture2D bunkerTextures[]);

#endif