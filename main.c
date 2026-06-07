#include "constants.h"
#include "structs.h"

#include "game_state.h"
#include "network.h"
#include "input.h"
#include "renderer.h"

#include "raylib.h"
#include <process.h>
#include <stdio.h>

GameState game;
CRITICAL_SECTION gameLock;
volatile int running = 1;

unsigned __stdcall ReceiveThread(void* arg);

int main() {

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Refactor Game");
    SetTargetFPS(60);

    SOCKET sock;
    network_init(&sock);

    InitializeCriticalSection(&gameLock);

    HANDLE thread = (HANDLE)_beginthreadex(
        NULL, 0, ReceiveThread, &sock, 0, NULL
    );

    int role = 1;
    int joined = 0;

    while (!WindowShouldClose() && running) {

        if (!joined) {
            network_send_join(sock, role);
            joined = 1;
        }

        input_handle(sock, role);

        BeginDrawing();
        ClearBackground(BLACK);

        EnterCriticalSection(&gameLock);
        GameState local = game;
        LeaveCriticalSection(&gameLock);

        DrawText(TextFormat("X=%d", local.player.cannon_x), 20, 20, 20, WHITE);

        EndDrawing();
    }

    running = 0;

    WaitForSingleObject(thread, INFINITE);

    CloseHandle(thread);
    DeleteCriticalSection(&gameLock);

    closesocket(sock);
    CloseWindow();

    return 0;
}

