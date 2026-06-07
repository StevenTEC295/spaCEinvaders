#ifndef ASSETS_H
#define ASSETS_H

#include "raylib.h"
#include "asset_ids.h"

void Assets_Init(void);
void Assets_Unload(void);

Texture2D GetTexture(TextureID id);

#endif
