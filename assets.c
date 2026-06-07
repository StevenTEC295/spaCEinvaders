#include "assets.h"
#include "constants.h"

#include <stdio.h>

static Texture2D textures[TEXTURE_COUNT];

void Assets_Init(void)
{
    textures[TEXTURE_PLAYER]   = LoadTexture("Assets/Ship.png");

    textures[TEXTURE_CRAB_1]   = LoadTexture("Assets/Crab1.png");
    textures[TEXTURE_CRAB_2]   = LoadTexture("Assets/Crab2.png");

    textures[TEXTURE_OCTO_1]   = LoadTexture("Assets/Octopus1.png");
    textures[TEXTURE_OCTO_2]   = LoadTexture("Assets/Octopus2.png");

    textures[TEXTURE_SQUID_1]  = LoadTexture("Assets/Squid1.png");
    textures[TEXTURE_SQUID_2]  = LoadTexture("Assets/Squid2.png");

    textures[TEXTURE_OVNI]     = LoadTexture("Assets/Ovni.png");

    textures[TEXTURE_BARRIER_0] = LoadTexture("Assets/Barrier.png");
    textures[TEXTURE_BARRIER_1] = LoadTexture("Assets/Barrier90.png");
    textures[TEXTURE_BARRIER_2] = LoadTexture("Assets/Barrier80.png");
    textures[TEXTURE_BARRIER_3] = LoadTexture("Assets/Barrier70.png");
    textures[TEXTURE_BARRIER_4] = LoadTexture("Assets/Barrier60.png");
    textures[TEXTURE_BARRIER_5] = LoadTexture("Assets/Barrier50.png");
    textures[TEXTURE_BARRIER_6] = LoadTexture("Assets/Barrier40.png");
    textures[TEXTURE_BARRIER_7] = LoadTexture("Assets/Barrier30.png");
    textures[TEXTURE_BARRIER_8] = LoadTexture("Assets/Barrier20.png");
    textures[TEXTURE_BARRIER_9] = LoadTexture("Assets/Barrier10.png");
}

Texture2D GetTexture(TextureID id)
{
    return textures[id];
}

void Assets_Unload(void)
{
    for (int i = 0; i < TEXTURE_COUNT; i++)
    {
        UnloadTexture(textures[i]);
    }
}