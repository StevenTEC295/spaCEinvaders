#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <winsock2.h>
#include <ws2tcpip.h>

#include "cJSON.h"
#include "structs.h"

#include "raylib.h"

#pragma comment(lib, "Ws2_32.lib")

#define SERVER_IP   "127.0.0.1"
#define SERVER_PORT 8080
#define BUFFER_SIZE 4096

#define SCREEN_WIDTH    800
#define SCREEN_HEIGHT   600

//==================================
// INTERFAZ
//==================================

// ============================
// ESTADO UI
// ============================
typedef enum {
    MENU,
    CONNECTED
} AppState;



//==================================


// liberar lista de aliens
void free_alien_list(AlienNode* head) {
    while (head) {
        AlienNode* tmp = head;
        head = head->next;
        free(tmp);
    }
}

// =====================================================
// Parser a JSON
// =====================================================

void process_message(const char* raw_json, GameState* state) {
    cJSON* root = cJSON_Parse(raw_json);
    if (!root) return;

    const char* type = cJSON_GetObjectItem(root, "type")->valuestring;

    if (strcmp(type, "GAME_STATE") == 0) {

        // ---------------- PLAYER ----------------
        cJSON* player = cJSON_GetObjectItem(root, "player");
        if (player) {
            cJSON* id = cJSON_GetObjectItem(player, "player_id");
            if (id) strncpy(state->player.player_id, id->valuestring, 9);

            state->player.cannon_x = cJSON_GetObjectItem(player, "cannon_x")->valueint;
            state->player.lives     = cJSON_GetObjectItem(player, "lives")->valueint;
            state->player.score     = cJSON_GetObjectItem(player, "score")->valueint;
        }

        // ---------------- ALIENS (LISTA ENLAZADA) ----------------
        cJSON* aliens = cJSON_GetObjectItem(root, "aliens");

        free_alien_list(state->aliens);
        state->aliens = NULL;

        if (aliens) {
            cJSON* alien;
            cJSON_ArrayForEach(alien, aliens) {

                AlienNode* node = malloc(sizeof(AlienNode));

                node->id  = cJSON_GetObjectItem(alien, "id")->valueint;
                node->x   = cJSON_GetObjectItem(alien, "x")->valueint;
                node->y   = cJSON_GetObjectItem(alien, "y")->valueint;
                node->pts = cJSON_GetObjectItem(alien, "pts")->valueint;

                cJSON* type = cJSON_GetObjectItem(alien, "type");
                if (type && type->valuestring)
                    strncpy(node->type, type->valuestring, 15);

                node->next = state->aliens;
                state->aliens = node;
            }
        }

        // ---------------- BULLETS ----------------
        cJSON* bullets = cJSON_GetObjectItem(root, "bullets");
        state->bullet_count = 0;

        if (bullets) {
            cJSON* b;
            cJSON_ArrayForEach(b, bullets) {
                if (state->bullet_count >= 100) break;

                state->bullets[state->bullet_count].x = cJSON_GetObjectItem(b, "x")->valueint;
                state->bullets[state->bullet_count].y = cJSON_GetObjectItem(b, "y")->valueint;
                state->bullets[state->bullet_count].active = true;

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
            state->ufo.active = cJSON_GetObjectItem(ufo, "active")->valueint;
            state->ufo.x      = cJSON_GetObjectItem(ufo, "x")->valueint;
            state->ufo.points = cJSON_GetObjectItem(ufo, "points")->valueint;
        }

        // ---------------- GAME META ----------------
        state->wave = cJSON_GetObjectItem(root, "wave")->valueint;

        cJSON* status = cJSON_GetObjectItem(root, "game_status");
        if (status && status->valuestring)
            strncpy(state->game_status, status->valuestring, 19);
    }

    cJSON_Delete(root);
}


int main() {

    // ============================
    // RAYLIB INIT
    // ============================
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Space Game Client");
    SetTargetFPS(60);

    AppState stateUI = MENU;
    int selectedRole = 0; // 1 jugador, 2 espectador

    WSADATA wsa;

    SOCKET sock;

    struct sockaddr_in server_addr;

    char buffer[BUFFER_SIZE];
    

    // =====================================================
    // INICIAR WINSOCK
    // =====================================================

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {

        printf("WSAStartup fallo\n");

        return 1;
    }

    // =====================================================
    // CREAR SOCKET
    // =====================================================

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (sock == INVALID_SOCKET) {

        printf("Error creando socket\n");

        WSACleanup();

        return 1;
    }

    // =====================================================
    // CONFIGURAR SERVIDOR
    // =====================================================

    server_addr.sin_family = AF_INET;

    server_addr.sin_port = htons(SERVER_PORT);

    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    // =====================================================
    // CONECTAR
    // =====================================================

    if (connect(sock,
        (struct sockaddr*)&server_addr,
        sizeof(server_addr)) == SOCKET_ERROR) {

        printf("Error conectando: %d\n",
            WSAGetLastError());

        closesocket(sock);

        WSACleanup();

        return 1;
    }

    printf("Conectado al servidor\n");

    // =====================================================
    // ENVIAR JOIN
    // =====================================================

    const char* join_msg =
        "{\"type\":\"JOIN\",\"player_name\":\"Jugador1\"}\n";

    send(sock,
        join_msg,
        (int)strlen(join_msg),
        0);

    printf("JOIN enviado\n");

    // =====================================================
    // LOOP PRINCIPAL
    // =====================================================

    int counter = 0;

    while (1) {

        memset(buffer, 0, BUFFER_SIZE);

        int bytes = recv(
            sock,
            buffer,
            BUFFER_SIZE - 1,
            0
        );

        if (bytes <= 0) {

            printf("Servidor desconectado\n");

            break;
        }

        buffer[bytes] = '\0';

        printf("\nMensaje recibido:\n%s\n", buffer);


        // =====================================================
        // ENVIAR SHOOT CADA 3 CICLOS
        // =====================================================

        counter++;

        if (counter >= 3) {

            const char* shoot_msg =
                "{\"type\":\"INPUT\",\"action\":\"SHOOT\"}\n";

            send(sock,
                shoot_msg,
                (int)strlen(shoot_msg),
                0);

            printf("SHOOT enviado\n");

            counter = 0;
        }

        Sleep(100);
    }

    // =====================================================
    // CERRAR
    // =====================================================

    closesocket(sock);

    WSACleanup();

    return 0;
}