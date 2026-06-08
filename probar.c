#include <stdio.h>
#include <winsock2.h>
#include <windows.h>


#include "include/network.h"
#include "include/game_state.h"

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
#define BUFFER_SIZE 4096

volatile int running = 1;

//Buffer compartido
SharedState state;
CRITICAL_SECTION cs;


//Hilo receptor (Solo recibir mensajes)
DWORD WINAPI network_thread (LPVOID arg){
    // ================= RECEIVE =================
    //Llamar para recibir datos
    SOCKET sock = *(SOCKET*)arg;
    
    char buffer[BUFFER_SIZE];

    while (running){

        int bytes = recv(sock, buffer, BUFFER_SIZE - 1, 0);

        if (bytes > 0) {
            //===Imprimir lo recibido por el server======
            buffer[bytes] = '\0';
            printf("MENSAJE SERVER:\n%s\n", buffer);
            
            
            //===Bloquear buffer a este hilo hasta que termine de copiar===
            EnterCriticalSection(&cs);
            // Sobrescribe el estado al más nuevo
            strcpy(state.buffer, buffer);
            state.has_new_data = 1;
            //Dejar de bloquear
            LeaveCriticalSection(&cs);
            
        } 
        else if (bytes == SOCKET_ERROR){
            printf("Servidor desconectado\n");
            running = 0;
            break;
        } 
        else{
            int err = WSAGetLastError();
            printf("Error recv: %d\n", err);
            running = 0;
            break;
            }
        }
    return 0;
    }


//Hilo principal (Renders, input y otros)
int main(void)
{
    //Llamar para recibir o enviar datos
    SOCKET sock = connect_server(SERVER_IP, SERVER_PORT);

    //Verificar conexión al servidor
    if (sock == INVALID_SOCKET)
    {
        printf("No se pudo conectar\n");
        return 1;
    }

    printf("Conexion exitosa\n");

    //Preguntar =============================
    HANDLE hNetwork = CreateThread(NULL,0,network_thread,&sock,0,NULL);

    //==============ESTADO========================
    //Prepara una sección en memoria para que solo se use en este hilo
    InitializeCriticalSection(&cs);

    //PARSER JSON
    while (running){
        GameState game = {0};

        //Bloquear buffer a solo este hilo hasta que termine de parsear
        EnterCriticalSection(&cs);
        
        if (state.has_new_data)
        {
            //Parser CJSON
            process_message(state.buffer, &game); 
            state.has_new_data = 0;
        }

        LeaveCriticalSection(&cs);

        //RENDER

    }
    
    DeleteCriticalSection(&cs);
    //Preguntar==========================
    WaitForSingleObject(hNetwork, INFINITE);
    CloseHandle(hNetwork);
    ;

    closesocket(sock);
    WSACleanup();

    return 0;
}