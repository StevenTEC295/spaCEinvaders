#include "network.h"
#include "constants.h"
#include <stdio.h>

void network_init(SOCKET* sock) {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);

    *sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server.sin_addr);

    connect(*sock, (struct sockaddr*)&server, sizeof(server));

    u_long mode = 1;
    ioctlsocket(*sock, FIONBIO, &mode);
}

void network_send_join(SOCKET sock, int role) {
    if (role == 1)
        send(sock,
        "{\"type\":\"JOIN\",\"role\":\"JUGADOR\",\"jugador_id\":\"P2\"}",
        60, 0);
    else
        send(sock,
        "{\"type\":\"JOIN\",\"player_name\":\"Espectador\"}",
        45, 0);
}

void network_send_input(SOCKET sock) {
    const char* msg = "{\"type\":\"SHOOT\"}";
    send(sock, msg, (int)strlen(msg), 0);
}