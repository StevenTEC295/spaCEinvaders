#include "input.h"
#include "raylib.h"
#include "network.h"

void input_handle(SOCKET sock, int role) {
    if (role != 1) return;

    if (IsKeyPressed(KEY_SPACE)) {
        network_send_input(sock);
    }
}
