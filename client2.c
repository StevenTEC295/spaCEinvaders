#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")
#include "cJSON.h"

#define SERVER_IP   "127.0.0.1"
#define SERVER_PORT 8080
#define BUFFER_SIZE 4096


int recv_all(SOCKET sock, char *buffer, int length)
{
    int total = 0;

    while (total < length)
    {
        int bytes = recv(sock, buffer + total, length - total, 0);
        if (bytes <= 0) return -1;
        total += bytes;
    }

    return total;
}

void convert_to_json(const char *filename, const char *content)
{
    cJSON *root = cJSON_CreateObject();

    cJSON_AddStringToObject(root, "file", filename);
    cJSON_AddStringToObject(root, "content", content);

    char *json = cJSON_Print(root);

    printf("\nJSON:\n%s\n", json);

    free(json);
    cJSON_Delete(root);
}

int main()
{
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server;

    WSAStartup(MAKEWORD(2,2), &wsa);

    sock = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server.sin_addr);

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0)
    {
        printf("Error conexión\n");
        return 1;
    }

    printf("Conectado al servidor\n");

    while (1)
    {
        uint32_t size_net;

        // 1. leer tamaño
        if (recv_all(sock, (char*)&size_net, 4) <= 0)
        {
            printf("Servidor desconectado\n");
            break;
        }

        uint32_t size = ntohl(size_net);

        // 2. leer mensaje completo
        char *buffer = (char*)malloc(size + 1);

        if (!buffer) break;

        if (recv_all(sock, buffer, size) <= 0)
        {
            free(buffer);
            break;
        }

        buffer[size] = '\0';

        // 3. separar filename | content
        char *sep = strchr(buffer, '|');

        if (sep)
        {
            *sep = '\0';

            char *filename = buffer;
            char *content = sep + 1;

            convert_to_json(filename, content);
        }

        free(buffer);
    }

    closesocket(sock);
    WSACleanup();

    return 0;
}