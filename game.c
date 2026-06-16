
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
#include "include/structs.h"
#include "include/constantes.h"
#include "include/input.h"

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

        printf("Esperando recv...\n");
        int bytes = recv(sock, buffer, BUFFER_SIZE - 1, 0);

        if (bytes > 0) {
            //===Imprimir lo recibido por el server======
            //buffer[bytes] = '\0';
            //printf("MENSAJE SERVER:\n%s\n", buffer);
            
            //===Bloquear buffer a este hilo hasta que termine de copiar===
            EnterCriticalSection(&cs);
            // Sobrescribe el estado al más nuevo
            strncpy(data.buffer, buffer, sizeof(data.buffer) - 1);
            data.buffer[sizeof(data.buffer) - 1] = '\0';

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
            if (err == WSAEINTR || !running) {
                break;
            }
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
    printf("Iniciando...\n");  // ← agrega esta línea
    fflush(stdout);  
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
    controller_init();

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
        WaveMessage.timer += GetFrameTime();

        if (alienTimer >= 0.5f)
        {
            alienFrame = !alienFrame;
            alienTimer = 0.0f;
        }

        //RENDER Dibujar el juego 
        DrawGame(&state, &role, &game, &assets, SCREEN_WIDTH, SCREEN_HEIGHT, alienFrame,sock);
        
        if (state == GAME_PLAYER){
            input_handle(sock, &role, &game);
        }
        

        //Bloquear buffer a solo este hilo hasta que termine de parsear
        EnterCriticalSection(&cs);
        
        if (data.has_new_data)
        {
            //Parser CJSON
            process_message(data.buffer, &game, &state); 
            data.has_new_data = 0;
        }
        LeaveCriticalSection(&cs);
        
    }
    DeleteCriticalSection(&cs);
    controller_close(); 

    printf("Saliendo del loop principal\n");

    running = 0;
    shutdown(sock, SD_BOTH);
    closesocket(sock);

    WaitForSingleObject(hNetwork, INFINITE);
    //Ejecuta el hilo receptor
    
    CloseHandle(hNetwork);

    //Librar memoria de los PNGs
    UnloadAssets(&assets); 
    CloseWindow();
    WSACleanup();
    return 0;
}