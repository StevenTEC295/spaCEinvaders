#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOUSER
#include "raylib.h"
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include "include/constantes.h"
#include "include/renderer.h"
#include "include/structs.h"
#include "include/assets.h"
#include "include/network.h"



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
            (current->x)*assets->aliens->width,
            (current->y)*assets->aliens->height,
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

    //printf("============================================JUGADOR:\n%d\n",game->bunkers->health);
    for (int i = 0; i < game->bunker_count; i++)
    {
        
        int hp = game->bunkers[i].health;

        if (hp <= 0)
            continue; // bunker destruido

        int index = (100 - hp) / 10;

        if (index < 0) index = 0;
        if (index > 9) index = 9;

        //x= , y= 612
        DrawTexture(
            assets->bunkers[index],
            ((game->bunkers[i].x)*60)+(assets->bunkers[0].width),
            (game->bunkers[i].y)+(ScreenHeight-(assets->player.height*4)), 
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
        if (strcmp(b->owner, "jugador") == 0 || strcmp(b->owner, "P1") == 0)
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
    if (game->ufo.exists)
    { 
        DrawTexture(assets->UFO,game->ufo.x * (assets->UFO.width),game->ufo.y * (assets->UFO.height),WHITE);
    }
}

//================================================
//============FUNCIÓN DIBUJAR JUGADOR ============
//================================================
void DrawPlayer (GameState* game, Assets *assets){
    DrawTexture(assets->player,game->player.cannon_x,game->player.cannon_y,WHITE);
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
//========FUNCIÓN DIBUJAR MENSAJE OLEADA =========
//================================================
void DrawWaves(GameState* game, Assets *assets, float ScreenWidth){
    // Variables
    
    if (game->wave != WaveMessage.lastWave){
        WaveMessage.active = true;
        WaveMessage.timer = 0.0f;
        WaveMessage.lastWave = game->wave; //Copia la oleada anterior
    }

    if (WaveMessage.active){
        if (WaveMessage.timer >= WaveMessage.duration){
            WaveMessage.active = false;
        }

    }

    if (WaveMessage.active){
        DrawText(TextFormat("OLEADA %i", (int)game->wave),(ScreenWidth - MeasureText(TextFormat("OLEADA %i", (int)game->wave), 30)) / 2, 100,30,YELLOW);
    }
 
}

//================================================
//============FUNCIÓN DIBUJAR MENÚ ===============
//================================================
void DrawMenu(AppState *state, UIEvent *role, float ScreenWidth, float ScreenHeight, SOCKET sock){
    // ================= MENU =================
    if (*state == MENU) {

        ClearBackground(BLACK);

        DrawText("SELECCIONA MODO", (ScreenWidth/2)-135, 250, 30, WHITE);
        
        // +60 +100
        Rectangle btn1 = {(ScreenWidth/2)-100, (ScreenHeight/2)-70, 200, 60};
        // = +100 del 1
        Rectangle btn2 = {(ScreenWidth/2)-100, (ScreenHeight/2)+30, 200, 60};

        //+50 +20 del boton 1
        DrawRectangleRec(btn1, DARKBLUE);
        DrawText("JUGADOR", (ScreenWidth/2)-45, (ScreenHeight/2)-50, 20, WHITE);

        //+30 +20 del boton 2
        DrawRectangleRec(btn2, DARKGRAY);
        DrawText("ESPECTADOR", (ScreenWidth/2)-65, (ScreenHeight/2)+50, 20, WHITE);

        Vector2 m = GetMousePosition();

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {

            if (CheckCollisionPointRec(m, btn1)) {
                *role = EVENT_JOIN_PLAYER;
                *state = GAME_PLAYER;
                network_send_join(sock, role); 
                
            }

            if (CheckCollisionPointRec(m, btn2)) {
                *role = EVENT_JOIN_SPECTATOR;
                *state = GAME_SPECTATOR;
                network_send_join(sock, role); 
            }
        }
    }
}

//Función principal de dibujo
void DrawGame(AppState *state, UIEvent *role, GameState *game, Assets *assets, float ScreenWidth, float ScreenHeight, int frame_time, SOCKET sock){
    BeginDrawing();

    switch (*state)
    {
        case MENU:
            DrawMenu(state, role, ScreenWidth, ScreenHeight, sock);
            break;

        case GAME_PLAYER:
            if (strcmp(game->game_status,"GAME_OVER") != 0) {
                ClearBackground(BLACK);
                DrawText("MODO JUGADOR", (ScreenWidth/2)-50, 0, 20, WHITE);
                DrawBunkers(game, assets, ScreenWidth, ScreenHeight);
                DrawAliens(game->aliens, frame_time, assets);
                DrawUFO(game, assets);
                DrawBullets(game, assets);
                DrawPlayer (game, assets);
                DrawScore(game);
                DrawLives(game, assets, ScreenWidth);
                DrawWaves(game, assets, ScreenWidth);
                
            } 

            break;

        case GAME_OVER:
            ClearBackground(DARKBLUE);
            DrawText("GAME OVER", (ScreenWidth - MeasureText("GAME OVER", 40))/2, 300, 40, RED);
            DrawText(game->gameOver.reason, (ScreenWidth - MeasureText(game->gameOver.reason, 20))/2, 360, 20, WHITE);
            char scoreText[64];
            sprintf(scoreText, "Score: %d", game->gameOver.final_score);
            DrawText(scoreText, (ScreenWidth - MeasureText(scoreText, 20))/2, 400, 20, WHITE);
            break;

        case GAME_SPECTATOR:
            ClearBackground(LIGHTGRAY);
            DrawText("MODO ESPECTADOR", ScreenWidth/2, 20, 20, WHITE);
            ClearBackground(DARKBLUE);
            DrawBunkers(game, assets, ScreenWidth, ScreenHeight);
            DrawAliens(game->aliens, frame_time, assets);
            DrawUFO(game, assets);
            DrawBullets(game, assets);
            DrawPlayer (game, assets);
            DrawScore(game);
            DrawLives(game, assets, ScreenWidth);
            break;
    }

    

    EndDrawing();
    
}
