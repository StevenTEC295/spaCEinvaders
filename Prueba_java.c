#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOUSER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <winsock2.h>
#include <ws2tcpip.h>

#include <windows.h>
#include <process.h>

#include "raylib.h"
#include "cJSON.h"
#include "structs.h"

//"127.0.0.1" Prueba
//"172.20.10.2" Para el Java Server
#define SERVER_IP "172.20.10.2"
#define SERVER_PORT 8080
#define BUFFER_SIZE 4096

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 900

#define NUM_BUNKERS 10
#define ALIEN_SPACING_X 64
#define ALIEN_SPACING_Y 48


GameState game = {0};

CRITICAL_SECTION gameLock;

volatile int running = 1;


// ================= UI STATE =================
typedef enum {
    MENU,
    GAME_PLAYER,
    GAME_SPECTATOR
} AppState;

/*
// ================= FREE LIST =================
void free_alien_list(AlienNode* head) {
    while (head) {
        AlienNode* tmp = head;
        head = head->next;
        free(tmp);
    }
}*/

//================================================
// ================= PARSER JSON =================
//================================================
void process_message(const char* raw_json, GameState* state) {
    printf("msj del server  %s\n", raw_json);
    cJSON* root = cJSON_Parse(raw_json);
    //if (!root) return;
    if (!root)
    {
        printf("\nERROR PARSEANDO JSON:\n");
        printf("%s\n", cJSON_GetErrorPtr());
        return;
    }

    cJSON* type = cJSON_GetObjectItem(root, "type");
    if (!type || !type->valuestring) {
        cJSON_Delete(root);
        return;
    }

    if (strcmp(type->valuestring, "GAME_STATE") == 0) {

        // ---------------- PLAYER ----------------
        cJSON* player = cJSON_GetObjectItem(root, "jugador");
        
        if (player) {
            cJSON* id = cJSON_GetObjectItem(player, "jugador_id");
            if (id && id->valuestring)
                strncpy(state->player.player_id, id->valuestring, 9);

            cJSON* cannon = cJSON_GetObjectItem(player, "cannon_x");
            if (cannon) state->player.cannon_x = cannon->valueint;

            cJSON* lives = cJSON_GetObjectItem(player, "vidas");
            if (lives) state->player.lives = lives->valueint;

            cJSON* score = cJSON_GetObjectItem(player, "score");
            if (score) state->player.score = score->valueint;
        }


        // ---------------- ALIENS (LISTA ENLAZADA) ----------------

        cJSON* aliens = cJSON_GetObjectItem(root, "aliens");

        free_alien_list(state->aliens);
        state->aliens = NULL;

        if (aliens) {
            cJSON* alien;
            cJSON_ArrayForEach(alien, aliens) {

                AlienNode* node = malloc(sizeof(AlienNode));

                cJSON* id = cJSON_GetObjectItem(alien, "id");
                cJSON* x  = cJSON_GetObjectItem(alien, "x");
                cJSON* y  = cJSON_GetObjectItem(alien, "y");
                cJSON* pts = cJSON_GetObjectItem(alien, "points");

                node->id  = id ? id->valueint : 0;
                node->x   = x  ? x->valueint  : 0;
                node->y   = y  ? y->valueint  : 0;
                node->pts = pts? pts->valueint: 0;
                
                // verifica el tipo de alien
                cJSON* t = cJSON_GetObjectItem(alien, "type");
                if (t && t->valuestring)
                {
                    if (strcmp(t->valuestring, "cangrejo") == 0)
                        node->alienType = ALIEN_CRAB;

                    else if (strcmp(t->valuestring, "pulpo") == 0)
                        node->alienType = ALIEN_OCTOPUS;

                    else if (strcmp(t->valuestring, "calamar") == 0)
                        node->alienType = ALIEN_SQUID;

                    else
                        node->alienType = ALIEN_UNKNOWN;
                }

                node->next = state->aliens;
                state->aliens = node;
            }
        }

        // ---------------- BULLETS ----------------
        cJSON* bullets = cJSON_GetObjectItem(root, "balas");
        state->bullet_count = 0;

        if (bullets) {
            cJSON* b;
            cJSON_ArrayForEach(b, bullets) {
                if (state->bullet_count >= 100) break;

                state->bullets[state->bullet_count].x = cJSON_GetObjectItem(b, "x")->valueint;
                state->bullets[state->bullet_count].y = cJSON_GetObjectItem(b, "y")->valueint;
                if (b && b->valuestring)
                    strncpy(state->bullets->owner, b->valuestring, 9);
                state->bullet_count++;

            }
        }

        // ---------------- BUNKERS ----------------
        cJSON* bunkers = cJSON_GetObjectItem(root, "bunkers");
        state->bunker_count = 0;

        if (bunkers) {
            cJSON* b;
            cJSON_ArrayForEach(b, bunkers) {
                if (state->bunker_count >= 4) break;

                state->bunkers[state->bunker_count].id = cJSON_GetObjectItem(b, "id")->valueint;
                state->bunkers[state->bunker_count].x = cJSON_GetObjectItem(b, "x")->valueint;
                state->bunkers[state->bunker_count].health = cJSON_GetObjectItem(b, "health")->valueint;

                state->bunker_count++;
            }
        }

        // ---------------- UFO ----------------
        cJSON* ufo = cJSON_GetObjectItem(root, "ufo");
        if (ufo) {
            state->ufo.active = cJSON_GetObjectItem(ufo, "activo")->valueint;
            state->ufo.x      = cJSON_GetObjectItem(ufo, "x")->valueint;
            state->ufo.points = cJSON_GetObjectItem(ufo, "points")->valueint;
        }

        // ---------------- GAME META ----------------
        cJSON* wave = cJSON_GetObjectItem(root, "wave");
        if (wave) state->wave = wave->valueint;

        cJSON* status = cJSON_GetObjectItem(root, "game_status");
        if (status && status->valuestring)
            strncpy(state->game_status, status->valuestring, 19);
    }

    cJSON_Delete(root);
}

//================================================
// FUNCIÓN DIBUJAR ALIENS (CON ANIMACIONES) ======
//================================================
void DrawAliens(AlienNode* aliens,int frame,Texture2D crab1,Texture2D crab2,Texture2D oct1,Texture2D oct2,Texture2D squid1,Texture2D squid2)
{
    AlienNode* current = aliens;

    while(current)
    {
        Texture2D texture;

        switch(current->alienType)
        {
            case ALIEN_CRAB:
                texture = frame ? crab1 : crab2;
                break;

            case ALIEN_OCTOPUS:
                texture = frame ? oct1 : oct2;
                break;

            case ALIEN_SQUID:
                texture = frame ? squid1 : squid2;
                break;
        }

        DrawTexture(
            texture,
            current->x* ALIEN_SPACING_X,
            current->y * ALIEN_SPACING_Y,
            WHITE
        );

        current = current->next;
    }
}

//================================================
//============FUNCIÓN DIBUJAR BUNKERS ============
//================================================
void DrawBunkers(GameState* game, Texture2D bunkerTextures[])
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
            bunkerTextures[index],
            game->bunkers[i].x,
            SCREEN_HEIGHT-200, 
            WHITE
        );
    }
}

// ================= INPUT ================= 
void handle_input(SOCKET sock, int role)
{
    if (role != 1) return;

    if (IsKeyPressed(KEY_SPACE)) {
        const char* msg = "{\"type\":\"SHOOT\"}";
        send(sock, msg, (int)strlen(msg), 0);
    }
}


unsigned __stdcall ReceiveThread(void* arg)
{
    SOCKET sock = *(SOCKET*)arg;

    char buffer[BUFFER_SIZE];

    while (running)
    {
        int bytes = recv(sock, buffer, BUFFER_SIZE - 1, 0);

        if (bytes > 0)
        {
            buffer[bytes] = '\0';

            printf("\n===== MENSAJE RECIBIDO =====\n");
            printf("%s\n", buffer);
            printf("============================\n");

            EnterCriticalSection(&gameLock);
            
            char recvBuffer[16384];
            int recvLen = 0;
            //process_message(buffer, &game);
            /*printf(
                "Jugador X=%d  vidas=%d  score=%d\n",
                game.player.cannon_x,
                game.player.lives,
                game.player.score
            );*/
            printf("RECIBIDOS %d BYTES\n", bytes);
            printf("%s\n", buffer);

            LeaveCriticalSection(&gameLock);
        }
        else if (bytes == 0)
        {
            printf("Servidor desconectado\n");
            running = 0;
            break;
        }
        else
        {
            int err = WSAGetLastError();

            if (err != WSAEWOULDBLOCK)
            {
                printf("Socket error: %d\n", err);
                running = 0;
                break;
            }

            Sleep(1);
        }
    }

    return 0;
}

// ================= MAIN =================
int main() {

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Raylib + Winsock + Parser");
    SetTargetFPS(60);
    int alienFrame = 0;
    float alienTimer = 0.0f;

    //Jugador
    Texture2D Img_Player = LoadTexture("Assets/Ship.png");

    //Barrera
    Texture2D bunkerTextures[10];

    bunkerTextures[0] = LoadTexture("Assets/Barrier.png");      
    bunkerTextures[1] = LoadTexture("Assets/Barrier90.png");    
    bunkerTextures[2] = LoadTexture("Assets/Barrier80.png");    
    bunkerTextures[3] = LoadTexture("Assets/Barrier70.png");    
    bunkerTextures[4] = LoadTexture("Assets/Barrier60.png");    
    bunkerTextures[5] = LoadTexture("Assets/Barrier50.png");    
    bunkerTextures[6] = LoadTexture("Assets/Barrier40.png");    
    bunkerTextures[7] = LoadTexture("Assets/Barrier30.png");    
    bunkerTextures[8] = LoadTexture("Assets/Barrier20.png");    
    bunkerTextures[9] = LoadTexture("Assets/Barrier10.png");

    //Aliens
    Texture2D Img_Crab1  = LoadTexture("Assets/Crab1.png");
    Texture2D Img_Crab2  = LoadTexture("Assets/Crab2.png");
    Texture2D Img_Octopus1  = LoadTexture("Assets/Octopus1.png");
    Texture2D Img_Octopus2  = LoadTexture("Assets/Octopus2.png");
    Texture2D Img_Squid1  = LoadTexture("Assets/Squid1.png");
    Texture2D Img_Squid2  = LoadTexture("Assets/Squid2.png");

    //OVNI
    Texture2D Img_Ovni    = LoadTexture("Assets/Ovni.png");

    // ================= SOCKET =================
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server_addr;

    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("WSAStartup fallo\n");
        return 1;
    }

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) return 1;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Error connect\n");
        return 1;
    }

    printf("Conectado\n");
    InitializeCriticalSection(&gameLock);

    
    // NON-BLOCKING SOCKET
    u_long mode = 1;
    ioctlsocket(sock, FIONBIO, &mode);

    HANDLE recvThread =
    (HANDLE)_beginthreadex(
        NULL,
        0,
        ReceiveThread,
        &sock,
        0,
        NULL
    );

    // ================= STATE =================
    AppState state = MENU;
    int role = 0;
    int joined = 0;

    char buffer[BUFFER_SIZE];
    //GameState game = {0};

    // ================= LOOP =================
    while (running && !WindowShouldClose()) {

        //Temporizador para la animación de los aliens
        alienTimer += GetFrameTime();

        if (alienTimer >= 0.5f)
        {
            alienFrame = !alienFrame;
            alienTimer = 0.0f;
        }

        BeginDrawing();

        // ================= MENU =================
        if (state == MENU) {

            ClearBackground(BLACK);

            DrawText("SELECCIONA MODO", 240, 100, 30, WHITE);

            Rectangle btn1 = {300, 200, 200, 60};
            Rectangle btn2 = {300, 300, 200, 60};

            DrawRectangleRec(btn1, DARKBLUE);
            DrawText("JUGADOR", 350, 220, 20, WHITE);

            DrawRectangleRec(btn2, DARKGRAY);
            DrawText("ESPECTADOR", 330, 320, 20, WHITE);

            Vector2 m = GetMousePosition();

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {

                if (CheckCollisionPointRec(m, btn1)) {
                    role = 1;
                    state = GAME_PLAYER;
                }

                if (CheckCollisionPointRec(m, btn2)) {
                    role = 2;
                    state = GAME_SPECTATOR;
                }
            }

            EndDrawing();
            continue;
        }

        // ================= BACKGROUND POR ESTADO =================
        if (state == GAME_PLAYER) {
            ClearBackground(DARKBLUE);
        } else {
            ClearBackground(DARKGRAY);
        }

        // ================= JOIN =================
        if (!joined) {

            //if (role == 1)
            if (role == 1)
                send(sock, "{\"type\":\"JOIN\",\"role\":\"JUGADOR\",\"jugador_id\":\"P2\"}\n", 55, 0);
            else
                send(sock, "{\"type\":\"JOIN\",\"player_name\":\"Espectador\"}\n", 50, 0);

            joined = 1;
        }

        // ================= INPUT ================= 
        
        if (state == GAME_PLAYER) 
        { 
            handle_input(sock, role); 
        }

        // ================= UI =================
        if (state == GAME_PLAYER) {

            EnterCriticalSection(&gameLock);

            // leer copia local (IMPORTANTE)
            GameState localGame = game;

            LeaveCriticalSection(&gameLock);

            DrawText("MODO JUGADOR", SCREEN_WIDTH/2, 20, 20, WHITE);
            
            //Bunker
            DrawBunkers(&game, bunkerTextures);

            //Jugador
            DrawText(TextFormat("X=%d", game.player.cannon_x),20,20,30,YELLOW);
            DrawTexture(Img_Player,game.player.cannon_x,SCREEN_HEIGHT-(Img_Player.height*1.5),WHITE);

            //Aliens
            DrawAliens(game.aliens,alienFrame,Img_Crab1,Img_Crab2,Img_Octopus1,Img_Octopus2,Img_Squid1,Img_Squid2);

            //Ovni 
            if (game.ufo.active == TRUE)
            { 
                DrawTexture(Img_Ovni,game.ufo.x,80,WHITE);
            }
            LeaveCriticalSection(&gameLock);
        }

        if (state == GAME_SPECTATOR) {
            DrawText("MODO ESPECTADOR", SCREEN_WIDTH/2, 20, 20, WHITE);
        }

        EndDrawing();
    }

    // ================= CLEANUP =================
    shutdown(sock, SD_BOTH);
    running = 0;
    shutdown(sock, SD_BOTH);
    WaitForSingleObject(recvThread, INFINITE);
    CloseHandle(recvThread);
    DeleteCriticalSection(&gameLock);
    closesocket(sock);
    WSACleanup();
    UnloadTexture(Img_Player);
    UnloadTexture(Img_Crab1);
    UnloadTexture(Img_Crab2);
    UnloadTexture(Img_Squid1);
    UnloadTexture(Img_Squid2);
    UnloadTexture(Img_Octopus1);
    UnloadTexture(Img_Octopus2);
    // lOOP para quitar las imagenes de los bunkers
    for (int i = 0; i < NUM_BUNKERS; i++) 
        {
            UnloadTexture(bunkerTextures[i]);
        }

    CloseWindow();

    return 0;
}