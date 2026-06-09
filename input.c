#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOUSER

#include "raylib.h"
#include "include/input.h"
#include "include/network.h"
#include "include/structs.h"
#include <stdio.h>

void input_handle(SOCKET sock, UIEvent *role) {
    if (*role == EVENT_NONE) return;

    if (IsKeyPressed(KEY_SPACE)) {
        network_send_shoot(sock);
    }
    if (IsKeyPressed(KEY_RIGHT)){
        network_send_right(sock);
    }
    if (IsKeyPressed(KEY_LEFT)){
        network_send_left(sock);
    }
}
