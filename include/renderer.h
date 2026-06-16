#ifndef RENDERER_H
#define RENDERER_H

#include "structs.h"
#include "raylib.h"
#include "assets.h"
#include <winsock2.h>

void DrawGame(AppState *state, UIEvent *role, GameState *game, Assets *assets, float ScreenWidth, float ScreenHeight, int frame_time, SOCKET sock);
void DrawMenu(AppState *state, UIEvent *role, float ScreenWidth, float ScreenHeight, SOCKET sock);
void DrawBunkers(GameState* game, Assets *assets, float ScreenWidth, float ScreenHeight);
void DrawAliens(AlienNode* aliens, int frame, Assets *assets);
void DrawUFO (GameState* game, Assets *assets);
void DrawBullets(GameState* game, Assets *assets);
void DrawPlayer (GameState* game, Assets *assets);
void DrawScore(GameState* game);
void DrawLives(GameState* game, Assets *assets, float ScreenWidth);
void DrawWaves(GameState* game, Assets *assets, float ScreenWidth);
void DrawViewerEntry(AppState *state, UIEvent *role,GameState *game, float ScreenWidth, float ScreenHeight, SOCKET sock);
#endif