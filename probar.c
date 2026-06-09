
#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOUSER

#include <raylib.h>
#include <stdio.h>
#include <winsock2.h>
#include <windows.h>

#include "include/network.h"
#include "include/game_state.h"
#include "include/renderer.h"
#include "include/assets.h"
#include "include/input.h"

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
#define BUFFER_SIZE 4096
#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 900

volatile int running = 1;

//Buffer compartido
SharedState data;
CRITICAL_SECTION cs;

volatile int joined = 0;

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
            //printf("MENSAJE SERVER:\n%s\n", buffer);
            
            //===Bloquear buffer a este hilo hasta que termine de copiar===
            EnterCriticalSection(&cs);
            // Sobrescribe el estado al más nuevo
            strcpy(data.buffer, buffer);
            data.has_new_data = 1;
            //Dejar de bloquear
            LeaveCriticalSection(&cs);
            
        } 
        else if (bytes == 0){
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
    //======================CONEXIÓN======================
    //Llamar para recibir o enviar datos
    SOCKET sock = connect_server(SERVER_IP, SERVER_PORT);

    //Verificar conexión al servidor
    if (sock == INVALID_SOCKET)
    {
        printf("No se pudo conectar\n");
        return 1;
    }

    printf("Conexion exitosa\n");

    //Prepara una sección en memoria para que solo se use en este hilo
    InitializeCriticalSection(&cs);
    //Llama al hilo receptor
    HANDLE hNetwork = CreateThread(NULL,0,network_thread,&sock,0,NULL);

    //=====================INTERFAZ=========================
    //VARIABLES
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Raylib + Winsock + Parser");
    SetTargetFPS(60);
    AppState state = MENU;
    UIEvent role = EVENT_NONE;
    Assets assets;
    //Cargar PNGs
    LoadAssets(&assets);
    //Variable de temporizador para animaciones
    int alienFrame = 0;
    float alienTimer = 0.0f;
    //int joined = 0;

    //==============ESTADO=======================
    GameState game = {0};
    printf("Entrando al loop principal\n");
    //PARSER JSON
    while (!WindowShouldClose()){
        
        //RENDER Temporizador animaciones
        alienTimer += GetFrameTime();

        if (alienTimer >= 0.5f)
        {
            alienFrame = !alienFrame;
            alienTimer = 0.0f;
        }

        //RENDER Dibujar el juego 
        DrawGame(&state, &role, &game, &assets, SCREEN_WIDTH, SCREEN_HEIGHT, alienFrame);

        //Enviar mensaje de join
        if (!joined && role != EVENT_NONE) {
            network_send_join(sock, &role);
            joined = 1;
        }  

        input_handle(sock, &role);

        //Bloquear buffer a solo este hilo hasta que termine de parsear
        EnterCriticalSection(&cs);
        
        if (data.has_new_data)
        {
            //Parser CJSON
            process_message(data.buffer, &game); 
            data.has_new_data = 0;
        }
        LeaveCriticalSection(&cs);
        
    }
    DeleteCriticalSection(&cs);

    printf("Saliendo del loop principal\n");

    running = 0;
    shutdown(sock, SD_BOTH);
    closesocket(sock);

    //Ejecuta el hilo receptor
    WaitForSingleObject(hNetwork, INFINITE);
    CloseHandle(hNetwork);

    //Librar memoria de los PNGs
    UnloadAssets(&assets); 
    CloseWindow();
    WSACleanup();
    return 0;
}