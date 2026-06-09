#include "include/renderer.h"
#include "include/structs.h"
#include "include/assets.h"
#include "raylib.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>


//================================================
// FUNCIÓN DIBUJAR ALIENS (CON ANIMACIONES) ======
//================================================
void DrawAliens(AlienNode* aliens, int frame, Assets *assets)
{
    AlienNode* current = aliens;
    
    while(current)
    {
        Texture2D texture;

        switch(current->alienType)
        {
            case ALIEN_CRAB:
                texture = frame ? assets->aliens[0] : assets->aliens[1];
                break;

            case ALIEN_OCTOPUS:
                texture = frame ? assets->aliens[2] : assets->aliens[3];
                break;

            case ALIEN_SQUID:
                texture = frame ? assets->aliens[4] : assets->aliens[5];
                break;
        }

        DrawTexture(
            texture,
            current->x,
            current->y,
            WHITE
        );

        current = current->next;
    }
}

//================================================
//============FUNCIÓN DIBUJAR BUNKERS ============
//================================================
void DrawBunkers(GameState* game, Assets *assets, float ScreenWidth, float ScreenHeight)
{
    for (int i = 0; i < game->bunker_count; i++)
    {
        int hp = game->bunkers[i].health;

        if (hp <= 0)
            continue; // bunker destruido

        int index = (100 - hp) / 10;

        if (index < 0) index = 0;
        if (index > 9) index = 9;

        DrawTexture(
            assets->bunkers[index],
            game->bunkers[i].x,
            ScreenHeight-250, 
            WHITE
        );
    }
}

//================================================
//============FUNCIÓN DIBUJAR BALAS ============
//================================================
void DrawBullets(GameState* game, Assets *assets)
{
    for (int i = 0; i < game->bullet_count; i++)
    {
        Bullet* b = &game->bullets[i];

        Texture2D tex;

        // Filtro según dueño
        if (strcmp(b->owner, "player") == 0 || strcmp(b->owner, "P1") == 0)
        {
            tex = assets->bullets[0];
        }
        else
        {
            tex = assets->bullets[1];
        }

        DrawTexture(tex, b->x, b->y, WHITE);
    }
}

//================================================
//============FUNCIÓN DIBUJAR OVNI ===============
//================================================
void DrawUFO (GameState* game, Assets *assets){
    if (game->ufo.active == true)
    { 
        DrawTexture(assets->UFO,game->ufo.x,80,WHITE);
    }
}

//================================================
//============FUNCIÓN DIBUJAR JUGADOR ============
//================================================
void DrawPlayer (GameState* game, Assets *assets, float ScreenHeight){
    DrawTexture(assets->player,game->player.cannon_x,ScreenHeight-((assets->player.height)*1.5),WHITE);
}

//================================================
//============FUNCIÓN DIBUJAR PUNTAJE ============
//================================================
void DrawScore(GameState* game){
    // Texto
    DrawText(TextFormat("SCORE: %i", (int)game->player.score), 10, 10, 30, WHITE);
}

//================================================
//============FUNCIÓN DIBUJAR VIDAS ==============
//================================================
void DrawLives(GameState* game, Assets *assets, float ScreenWidth){
    // Variables
    float x = ScreenWidth-(ScreenWidth/4);
    float y = 10;
    float size = 70;
    float size_y = 40;
    // Texto
    DrawText(("LIVES: "), x-100, y, 30, WHITE);
    
    //Dibuja las imagenes de las vidas
    for (int i = 0; i < game->player.lives; i++)
    {
        //Cada 4 vidas, se inserta la imagen del siguiente abajo del primero de la fila anterior
        int col = i % 4;
        int row = i / 4;

        DrawTexture(
            assets->Lives,
            (x + (col * size))+5,
            (y + (row * size_y))-10, 
            WHITE
        );
    }

}

//================================================
//============FUNCIÓN DIBUJAR MENÚ ===============
//================================================
void DrawMenu(AppState *state, UIEvent *role, float ScreenWidth, float ScreenHeight){
    // ================= MENU =================
    if (*state == MENU) {

        ClearBackground(BLACK);

        DrawText("SELECCIONA MODO", (ScreenWidth/2)-225, 100, 30, WHITE);
        
        // +60 +100
        Rectangle btn1 = {(ScreenWidth/2)-200, (ScreenHeight/2)-60, 200, 60};
        // = +100 del 1
        Rectangle btn2 = {(ScreenWidth/2)-200, (ScreenHeight/2)+40, 200, 60};

        //+50 +20 del boton 1
        DrawRectangleRec(btn1, DARKBLUE);
        DrawText("JUGADOR", (ScreenWidth/2)-175, (ScreenHeight/2)-40, 20, WHITE);

        //+30 +20 del boton 2
        DrawRectangleRec(btn2, DARKGRAY);
        DrawText("ESPECTADOR", (ScreenWidth/2)-170, (ScreenHeight/2)+60, 20, WHITE);

        Vector2 m = GetMousePosition();

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {

            if (CheckCollisionPointRec(m, btn1)) {
                *role = EVENT_JOIN_PLAYER;
                *state = GAME_PLAYER;
            }

            if (CheckCollisionPointRec(m, btn2)) {
                *role = EVENT_JOIN_SPECTATOR;
                *state = GAME_SPECTATOR;
            }
        }
    }
}

//Función principal de dibujo
void DrawGame(AppState *state, UIEvent *role, GameState *game, Assets *assets, float ScreenWidth, float ScreenHeight, int frame_time){
    BeginDrawing();

    switch (*state)
    {
        case MENU:
            DrawMenu(state, role, ScreenWidth, ScreenHeight);
            break;

        case GAME_PLAYER:
            if (strcmp(game->game_status,"GAME_OVER") != 0) {
                ClearBackground(DARKBLUE);
                DrawText("MODO JUGADOR", (ScreenWidth/2)-50, 0, 20, WHITE);
                DrawBunkers(game, assets, ScreenWidth, ScreenHeight);
                DrawAliens(game->aliens, frame_time, assets);
                DrawUFO(game, assets);
                DrawBullets(game, assets);
                DrawPlayer (game, assets, ScreenHeight);
                DrawScore(game);
                DrawLives(game, assets, ScreenWidth);
            } else {
                DrawText("GAME OVER", 300, 200, 40, RED);
            }

            break;

        case GAME_SPECTATOR:
            ClearBackground(LIGHTGRAY);
            DrawText("MODO ESPECTADOR", ScreenWidth/2, 20, 20, WHITE);
            break;
    }

    EndDrawing();
    
}
