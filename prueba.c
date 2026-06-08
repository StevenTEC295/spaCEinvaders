#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOUSER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <winsock2.h>
#include <ws2tcpip.h>

#include "raylib.h"
#include "include/cJSON.h"
#include "include/structs.h"

//"127.0.0.1" Prueba
//"172.20.10.2" Para el Java Server
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
#define BUFFER_SIZE 4096

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 900

#define NUM_BUNKERS 10

// ================= UI STATE =================
typedef enum {
    MENU,
    GAME_PLAYER,
    GAME_SPECTATOR
} AppState;

// ================= FREE LIST =================
void free_alien_list(AlienNode* head) {
    while (head) {
        AlienNode* tmp = head;
        head = head->next;
        free(tmp);
    }
}

//================================================
// ================= PARSER JSON =================
//================================================
void process_message(const char* raw_json, GameState* state) {

    //Convierte el texto en un árbol de datos CJSON
    cJSON* root = cJSON_Parse(raw_json);
    //Mensaje de error en caso de no parsear correctamente
    if (!root)
    {
        printf("\nERROR AL PARSEAR JSON:\n");
        printf("%s\n", cJSON_GetErrorPtr());
        return;
    }

    //Busca "type" en el JSON 
    cJSON* type = cJSON_GetObjectItem(root, "type");

    //Validar que "type" exista y que sea una cadena
    if (!type || !type->valuestring) {
        cJSON_Delete(root);
        return;
    }

    // Procesar el mensaje de JSON de "GAME_STATE"
    if (strcmp(type->valuestring, "GAME_STATE") == 0) {

        // ---------------- PLAYER ----------------
        //Busca el objeto "player"
        cJSON* player = cJSON_GetObjectItem(root, "player");
        
        if (player) {
            //Busca y copia el ID del jugador a los structs (MAX 9 caracteres)
            cJSON* id = cJSON_GetObjectItem(player, "player_id");
            if (id && id->valuestring)
                strncpy(state->player.player_id, id->valuestring, 9);
            
            //Busca y designa la ubicación del cañon del jugador en valores numéricos a los structs
            cJSON* cannon = cJSON_GetObjectItem(player, "cannon_x");
            if (cannon) state->player.cannon_x = cannon->valueint;
            
            //Busca y designa la cantidad de vidas del jugador en valores numéricos a los structs
            cJSON* lives = cJSON_GetObjectItem(player, "lives");
            if (lives) state->player.lives = lives->valueint;

            //Busca y designa el puntaje del jugador en valores numéricos a los structs
            cJSON* score = cJSON_GetObjectItem(player, "score");
            if (score) state->player.score = score->valueint;
        }


        // ---------------- ALIENS (LISTA ENLAZADA) ----------------

        //Busca el objeto "aliens"
        cJSON* aliens = cJSON_GetObjectItem(root, "aliens");

        //Borra la lista anterior. Liberar nodos de listas anteriores (Quita acumulación de memoria)
        free_alien_list(state->aliens);
        state->aliens = NULL;
    
        if (aliens) {
            cJSON* alien;

            //Recorre array de aliens
            cJSON_ArrayForEach(alien, aliens) {

                //Reservar memoria a un alien del array
                AlienNode* node = malloc(sizeof(AlienNode));
                
                cJSON* id = cJSON_GetObjectItem(alien, "id");
                cJSON* x  = cJSON_GetObjectItem(alien, "x");
                cJSON* y  = cJSON_GetObjectItem(alien, "y");
                cJSON* pts = cJSON_GetObjectItem(alien, "pts");
                
                //Designa todos los atributos a los structs, da 0 si están vacíos
                node->id  = id ? id->valueint : 0;
                node->x   = x  ? x->valueint  : 0;
                node->y   = y  ? y->valueint  : 0;
                node->pts = pts? pts->valueint: 0;
                
                //Verifica el tipo de alien
                cJSON* t = cJSON_GetObjectItem(alien, "type");
                if (t && t->valuestring)
                {
                    if (strcmp(t->valuestring, "crab") == 0)
                        node->alienType = ALIEN_CRAB;

                    else if (strcmp(t->valuestring, "octopus") == 0)
                        node->alienType = ALIEN_OCTOPUS;

                    else if (strcmp(t->valuestring, "squid") == 0)
                        node->alienType = ALIEN_SQUID;

                    else
                        node->alienType = ALIEN_UNKNOWN;
                }

                //Inserta alien en el array (Inserta al inicio de lista)
                node->next = state->aliens;
                state->aliens = node;
            }
        }

        // ---------------- BULLETS ----------------
        //Busca el objeto "bullets"
        cJSON* bullets = cJSON_GetObjectItem(root, "bullets");
        //Contador de balas 
        state->bullet_count = 0;

        if (bullets) {
            cJSON* b;
            
            //Recorre array de balas
            cJSON_ArrayForEach(b, bullets) {
                //Termina el recorrido una vez que se alcanza el limite
                if (state->bullet_count >= 100) break;

                //Designa todos los atributos a los structs (x,y)
                cJSON* x = cJSON_GetObjectItem(b, "x");
                cJSON* y = cJSON_GetObjectItem(b, "y");

                if (!cJSON_IsNumber(x) || !cJSON_IsNumber(y))
                    continue;

                int idx = state->bullet_count;

                state->bullets[idx].x = x->valueint;
                state->bullets[idx].y = y->valueint;
                
                state->bullets[idx].owner[0] = '\0';
                //Designa todos los atributos a los structs (owner)
                cJSON* owner = cJSON_GetObjectItem(b, "owner");
                

                if (cJSON_IsString(owner) && owner->valuestring)
                {
                    strncpy(state->bullets[idx].owner,owner->valuestring,
                        sizeof(state->bullets[idx].owner) - 1);

                    state->bullets[idx].owner[sizeof(state->bullets[idx].owner) - 1] = '\0';
                }
                //Incrementa el contador 
                state->bullet_count++;
            }
        }

        // ---------------- BUNKERS ----------------
        //Busca el objeto "bunkers"
        cJSON* bunkers = cJSON_GetObjectItem(root, "bunkers");
        //Contador de bunkers
        state->bunker_count = 0;

        if (bunkers) {
            cJSON* b;
            cJSON_ArrayForEach(b, bunkers) {
                //Termina el recorrido una vez que se alcanza el limite
                if (state->bunker_count >= 4) break;
                
                //Designa todos los atributos a los structs
                state->bunkers[state->bunker_count].id = cJSON_GetObjectItem(b, "id")->valueint;
                state->bunkers[state->bunker_count].x = cJSON_GetObjectItem(b, "x")->valueint;
                state->bunkers[state->bunker_count].health = cJSON_GetObjectItem(b, "health")->valueint;

                //Incrementa el contador 
                state->bunker_count++;
            }
        }

        // ---------------- UFO ----------------
        //Busca el objeto "ufo"
        cJSON* ufo = cJSON_GetObjectItem(root, "ufo");
        if (ufo) {
            //Designa todos los atributos a los structs
            state->ufo.active = cJSON_GetObjectItem(ufo, "active")->valueint;
            state->ufo.x      = cJSON_GetObjectItem(ufo, "x")->valueint;
            state->ufo.points = cJSON_GetObjectItem(ufo, "points")->valueint;
        }

        // ---------------- GAME META ----------------
        //Busca el objeto "wave"
        cJSON* wave = cJSON_GetObjectItem(root, "wave");
        //Designa el valor númerico de la oleada a los structs
        if (wave) state->wave = wave->valueint;

        //Busca el objeto "game_status"
        cJSON* status = cJSON_GetObjectItem(root, "game_status");
        //Designa el string de estado del juego a los structs
        if (status && status->valuestring)
            strncpy(state->game_status, status->valuestring, 19);
    }
    //Libera el árbol JSON (Liberar memoria reservada)
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

//================================================
//============FUNCIÓN DIBUJAR BALAS ============
//================================================
void DrawBullets(GameState* game, Texture2D playerBullet, Texture2D alienBullet)
{
    for (int i = 0; i < game->bullet_count; i++)
    {
        Bullet* b = &game->bullets[i];

        Texture2D tex;

        // Filtro según dueño
        if (strcmp(b->owner, "player") == 0 || strcmp(b->owner, "P1") == 0)
        {
            tex = playerBullet;
        }
        else
        {
            tex = alienBullet;
        }

        DrawTexture(tex, b->x, b->y, WHITE);
    }
}

// ================= INPUT ================= 
void handle_input(SOCKET sock, int role)
{
    if (role != 1) return;

    if (IsKeyPressed(KEY_SPACE)) {
        const char* msg = "{\"type\":\"SHOOT\"}\n";
        send(sock, msg, (int)strlen(msg), 0);
    }
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

    //Balas 
    Texture2D Img_alien_bullet = LoadTexture("Assets/Alien_bullet.png");
    Texture2D Img_player_bullet = LoadTexture("Assets/Player_bullet.png");

    // ================= SOCKET =================
    //Declarar variables
    WSADATA wsa; //almacena info
    SOCKET sock; //Socket
    struct sockaddr_in server_addr; //Dirección IPv4

    //Inicializa Winsock
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("WSAStartup fallo\n");
        return 1;
    }

    //Crear Socket
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //Verificación de creación de Socket
    if (sock == INVALID_SOCKET) return 1;

    //==Configuración== 
    //Dirección de servidor
    server_addr.sin_family = AF_INET;
    //Puerto
    server_addr.sin_port = htons(SERVER_PORT);
    //Dirección IP
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    //Conexión al servidor
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Error connect\n");
        return 1;
    }

    printf("Conectado\n");

    //Socket no bloqueante (Evitar que el juego se bloquee esperando mensajes)
    u_long mode = 1;
    ioctlsocket(sock, FIONBIO, &mode);

    // ================= STATE =================
    AppState state = MENU;
    int role = 0;
    int joined = 0;

    char buffer[BUFFER_SIZE];
    GameState game = {0};

    // ================= LOOP =================
    while (!WindowShouldClose()) {

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
                send(sock, "{\"type\":\"JOIN\",\"role\":\"JUGADOR\",\"jugador_id\":\"P1\"}", 53, 0);
            else
                send(sock, "{\"type\":\"JOIN\",\"player_name\":\"Espectador\"}\n", 50, 0);

            joined = 1;
        }

        // ================= RECEIVE =================
        int bytes = recv(sock, buffer, BUFFER_SIZE - 1, 0);

        if (bytes > 0) {
            buffer[bytes] = '\0';
            process_message(buffer, &game);
            printf("Mensaje recibido:\n%s\n", buffer);
        }
        else if (bytes == SOCKET_ERROR) {
            int err = WSAGetLastError();
            if (err != WSAEWOULDBLOCK) {
                printf("Socket error: %d\n", err);
            }
        }

        // ================= INPUT ================= 
        
        if (state == GAME_PLAYER) 
        { 
            handle_input(sock, role); 
        }

        // ================= UI =================
        if (state == GAME_PLAYER) {
            DrawText("MODO JUGADOR", SCREEN_WIDTH/2, 10, 20, WHITE);
            
            //Bunker
            DrawBunkers(&game, bunkerTextures);

            //Jugador
            DrawTexture(Img_Player,game.player.cannon_x,SCREEN_HEIGHT-(Img_Player.height*1.5),WHITE);

            //Aliens
            DrawAliens(game.aliens,alienFrame,Img_Crab1,Img_Crab2,Img_Octopus1,Img_Octopus2,Img_Squid1,Img_Squid2);

            //Ovni 
            if (game.ufo.active == TRUE)
            { 
                DrawTexture(Img_Ovni,game.ufo.x,80,WHITE);
            }
            //Balas
            DrawBullets(&game, Img_player_bullet, Img_alien_bullet);
        }

        if (state == GAME_SPECTATOR) {
            DrawText("MODO ESPECTADOR", SCREEN_WIDTH/2, 20, 20, WHITE);
        }

        EndDrawing();
    }

    // ================= CLEANUP =================
    shutdown(sock, SD_BOTH);
    closesocket(sock);
    WSACleanup();
    UnloadTexture(Img_Player);
    UnloadTexture(Img_Crab1);
    UnloadTexture(Img_Crab2);
    UnloadTexture(Img_Squid1);
    UnloadTexture(Img_Squid2);
    UnloadTexture(Img_Octopus1);
    UnloadTexture(Img_Octopus2);
    UnloadTexture(Img_alien_bullet);
    UnloadTexture(Img_player_bullet);
    // lOOP para quitar las imagenes de los bunkers
    for (int i = 0; i < NUM_BUNKERS; i++) 
        {
            UnloadTexture(bunkerTextures[i]);
        }

    CloseWindow();

    return 0;
}