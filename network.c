#include "include/network.h"
#include <stdio.h>

#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOUSER

#include <winsock2.h>
#include <ws2tcpip.h>

//Conexión al servidor
SOCKET connect_server(const char *ip, int port) {
    
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
    server_addr.sin_port = htons(port);
    //Dirección IP
    inet_pton(AF_INET, ip, &server_addr.sin_addr);

    //Conexión al servidor
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Error connect\n");
        return 1;
    }

    printf("Conectado\n");

    //Socket no bloqueante (Evitar que el juego se bloquee esperando mensajes)
    //u_long mode = 1;
    //ioctlsocket(sock, FIONBIO, &mode);

    return sock;
}

//Envío de mensajes al escoger Jugador o Espectador

/*
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

//Inputs

void network_send_input(SOCKET sock) {
    const char* msg = "{\"type\":\"SHOOT\"}";
    send(sock, msg, (int)strlen(msg), 0);
}*/