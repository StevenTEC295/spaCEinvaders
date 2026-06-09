#include "include/assets.h"
#include <stdio.h>

bool LoadAssets(Assets *assets)
{
    assets->player = LoadTexture("Assets/Ship.png");
    assets->UFO = LoadTexture("Assets/Ovni.png");
    assets->bunkers[0] = LoadTexture("Assets/Barrier.png");
    assets->bunkers[1] = LoadTexture("Assets/Barrier90.png");
    assets->bunkers[2] = LoadTexture("Assets/Barrier80.png");
    assets->bunkers[3] = LoadTexture("Assets/Barrier70.png");
    assets->bunkers[4] = LoadTexture("Assets/Barrier60.png");
    assets->bunkers[5] = LoadTexture("Assets/Barrier50.png");
    assets->bunkers[6] = LoadTexture("Assets/Barrier40.png");
    assets->bunkers[7] = LoadTexture("Assets/Barrier30.png");
    assets->bunkers[8] = LoadTexture("Assets/Barrier20.png");
    assets->bunkers[9] = LoadTexture("Assets/Barrier10.png");
    assets->aliens[0] = LoadTexture("Assets/Crab1.png");
    assets->aliens[1] = LoadTexture("Assets/Crab2.png");
    assets->aliens[2] = LoadTexture("Assets/Octopus1.png");
    assets->aliens[3] = LoadTexture("Assets/Octopus2.png");
    assets->aliens[4] = LoadTexture("Assets/Squid1.png");
    assets->aliens[5] = LoadTexture("Assets/Squid2.png");
    assets->bullets[0] = LoadTexture("Assets/Player_bullet.png");
    assets->bullets[1] = LoadTexture("Assets/Alien_bullet.png");

    return true;
}

void UnloadAssets(Assets *assets)
{
    UnloadTexture(assets->player);
    UnloadTexture(assets->UFO);
    // lOOP para quitar las imagenes de los bunkers y aliens
    for (int i = 0; i < 10; i++) 
    {
        UnloadTexture(assets->bunkers[i]);
    }
    
    for (int i = 0; i < 6; i++) 
    {
        UnloadTexture(assets->aliens[i]);
    }
    for (int i = 0; i < 2; i++) 
    {
        UnloadTexture(assets->bullets[i]);
    }
    
}
