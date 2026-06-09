#ifndef ASSETS_H
#define ASSETS_H

#include "raylib.h"

//Struct para los PNGs
typedef struct {
    Texture2D player;
    Texture2D aliens[6]; //Cantidad de .PNGs de aliens
    Texture2D bullets[2]; //Cantidad de .PNGs de bullets
    Texture2D UFO;
    Texture2D bunkers[10]; //Cantidad de .PNGs de Bunkers
} Assets;

bool LoadAssets(Assets *assets);
void UnloadAssets(Assets *assets);

#endif
