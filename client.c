#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

#define SERVER_IP   "127.0.0.1"
#define SERVER_PORT 8080
#define BUFFER_SIZE 4096

int main() {

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
        printf("Error creando socket: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // =====================================================
    // CONFIGURAR DIRECCION
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

        printf("Error conectando: %d\n", WSAGetLastError());

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

    send(sock, join_msg, (int)strlen(join_msg), 0);

    printf("JOIN enviado\n");

    // =====================================================
    // RECIBIR JOIN_ACK
    // =====================================================

    memset(buffer, 0, BUFFER_SIZE);

    int bytes = recv(sock, buffer, BUFFER_SIZE - 1, 0);

    if (bytes > 0) {

        buffer[bytes] = '\0';

        printf("Servidor respondio:\n%s\n", buffer);
    }

    // =====================================================
    // LOOP PRINCIPAL
    // =====================================================

    int counter = 0;

    while (1) {

        memset(buffer, 0, BUFFER_SIZE);

        bytes = recv(sock, buffer, BUFFER_SIZE - 1, 0);

        if (bytes <= 0) {

            printf("Servidor desconectado\n");

            break;
        }

        buffer[bytes] = '\0';

        printf("Mensaje recibido:\n%s\n", buffer);

        counter++;

        // Enviar SHOOT cada 20 ciclos
        if (counter >= 20) {

            const char* shoot_msg =
                "{\"type\":\"INPUT\",\"action\":\"SHOOT\"}\n";

            send(sock, shoot_msg,
                (int)strlen(shoot_msg), 0);

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