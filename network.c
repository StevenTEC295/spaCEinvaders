#include "include/network.h"
#include "include/structs.h"
#include <stdio.h>

#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOUSER

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

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
        //return 1;
        return INVALID_SOCKET;
    }

    //Crear Socket
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //Verificación de creación de Socket
    if (sock == INVALID_SOCKET) return INVALID_SOCKET;//return 1;

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
        //return 1;
        return INVALID_SOCKET;
    }

    printf("Conectado\n");

    //Socket no bloqueante (Evitar que el juego se bloquee esperando mensajes)
    u_long mode = 0;
    ioctlsocket(sock, FIONBIO, &mode);

    return sock;
}

//Envío de mensajes al escoger Jugador o Espectador
void network_send_join(SOCKET sock, UIEvent *role)
{
    int result;

    printf("Role = %d\n", *role);
    if (*role == EVENT_JOIN_PLAYER)
    {
        const char *msg =
            "{\"type\":\"JOIN\",\"role\":\"JUGADOR\",\"jugador_id\":\"P1\"}\n";

        result = send(sock, msg, strlen(msg), 0);
        
    }
    else if (*role == EVENT_JOIN_SPECTATOR)
    {
        const char *msg =
            "{\"type\":\"JOIN\",\"role\":\"SPECTATOR\",\"watch_player\":\"P2\"}\n";;

        result = send(sock, msg, strlen(msg), 0);
    }
    else
    {
        return;
    }

    if (result == SOCKET_ERROR)
    {
        printf("Error enviando JOIN: %d\n", WSAGetLastError());
    }
}

//==========Inputs==============
//Disparar
void network_send_shoot(SOCKET sock)
{
    const char* msg = "{\"type\":\"SHOOT\"}\n";

    int result = send(sock, msg, strlen(msg), 0);

    if (result == SOCKET_ERROR)
    {
        printf("Error SHOOT: %d\n", WSAGetLastError());
    }
}
//Derecha
void network_send_right(SOCKET sock)
{
    const char* msg = "{\"type\":\"MOVE\",\"direction\":\"RIGHT\"}\n";

    int result = send(sock, msg, strlen(msg), 0);

    if (result == SOCKET_ERROR)
    {
        printf("Error SHOOT: %d\n", WSAGetLastError());
    }
}
//Izquierda
void network_send_left(SOCKET sock)
{
    const char* msg = "{\"type\":\"MOVE\",\"direction\":\"LEFT\"}\n";

    int result = send(sock, msg, strlen(msg), 0);

    if (result == SOCKET_ERROR)
    {
        printf("Error SHOOT: %d\n", WSAGetLastError());
    }
}