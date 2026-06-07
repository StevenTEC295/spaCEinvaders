#include "renderer.h"
#include "constants.h"

void draw_aliens(AlienNode* aliens, int frame,
                 Texture2D crab1, Texture2D crab2,
                 Texture2D oct1, Texture2D oct2,
                 Texture2D sq1, Texture2D sq2)
{
    while (aliens) {

        Texture2D tex;

        switch (aliens->alienType) {
            case ALIEN_CRAB:
                tex = frame ? crab1 : crab2;
                break;
            case ALIEN_OCTOPUS:
                tex = frame ? oct1 : oct2;
                break;
            case ALIEN_SQUID:
                tex = frame ? sq1 : sq2;
                break;
            default:
                tex = crab1;
        }

        DrawTexture(tex,
            aliens->x * ALIEN_SPACING_X,
            aliens->y * ALIEN_SPACING_Y,
            WHITE);

        aliens = aliens->next;
    }
}

void draw_bunkers(GameState* game, Texture2D bunkerTextures[])
{
    for (int i = 0; i < game->bunker_count; i++) {

        int hp = game->bunkers[i].health;
        if (hp <= 0) continue;

        int idx = (100 - hp) / 10;
        if (idx < 0) idx = 0;
        if (idx > 9) idx = 9;

        DrawTexture(
            bunkerTextures[idx],
            game->bunkers[i].x,
            SCREEN_HEIGHT - 200,
            WHITE
        );
    }
}
