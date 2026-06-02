#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOUSER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <winsock2.h>
#include <ws2tcpip.h>

#include "raylib.h"
#include "cJSON.h"
#include "structs.h"

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
#define BUFFER_SIZE 4096

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

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

// ================= PARSER JSON =================
void process_message(const char* raw_json, GameState* state) {

    cJSON* root = cJSON_Parse(raw_json);
    if (!root) return;

    cJSON* type = cJSON_GetObjectItem(root, "type");
    if (!type || !type->valuestring) {
        cJSON_Delete(root);
        return;
    }

    if (strcmp(type->valuestring, "GAME_STATE") == 0) {

        cJSON* player = cJSON_GetObjectItem(root, "player");

        if (player) {
            cJSON* id = cJSON_GetObjectItem(player, "player_id");
            if (id && id->valuestring)
                strncpy(state->player.player_id, id->valuestring, 9);

            cJSON* cannon = cJSON_GetObjectItem(player, "cannon_x");
            if (cannon) state->player.cannon_x = cannon->valueint;

            cJSON* lives = cJSON_GetObjectItem(player, "lives");
            if (lives) state->player.lives = lives->valueint;

            cJSON* score = cJSON_GetObjectItem(player, "score");
            if (score) state->player.score = score->valueint;
        }

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
                cJSON* pts = cJSON_GetObjectItem(alien, "pts");

                node->id  = id ? id->valueint : 0;
                node->x   = x  ? x->valueint  : 0;
                node->y   = y  ? y->valueint  : 0;
                node->pts = pts? pts->valueint: 0;

                cJSON* t = cJSON_GetObjectItem(alien, "type");
                if (t && t->valuestring)
                    strncpy(node->type, t->valuestring, 15);

                node->next = state->aliens;
                state->aliens = node;
            }
        }

        cJSON* wave = cJSON_GetObjectItem(root, "wave");
        if (wave) state->wave = wave->valueint;

        cJSON* status = cJSON_GetObjectItem(root, "game_status");
        if (status && status->valuestring)
            strncpy(state->game_status, status->valuestring, 19);
    }

    cJSON_Delete(root);
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

    // NON-BLOCKING SOCKET
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

            if (role == 1)
                send(sock, "{\"type\":\"JOIN\",\"player_name\":\"Jugador1\"}\n", 45, 0);
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
        if (state == GAME_PLAYER) {
            handle_input(sock, role);
        }

        // ================= UI =================
        if (state == GAME_PLAYER) {
            DrawText("MODO JUGADOR", 300, 20, 20, WHITE);
        }

        if (state == GAME_SPECTATOR) {
            DrawText("MODO ESPECTADOR", 300, 20, 20, WHITE);
        }

        EndDrawing();
    }

    // ================= CLEANUP =================
    closesocket(sock);
    WSACleanup();
    CloseWindow();

    return 0;
}