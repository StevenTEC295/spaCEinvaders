#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOUSER
#include <synchapi.h>
#include "raylib.h"
#include "include/input.h"
#include "include/network.h"
#include "include/structs.h"
#include "include/game_state.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <windows.h>

// ── Configuración del control ─────────────────────────────────
#define CONTROLLER_PORT   "COM8"
#define CONTROLLER_BAUD   115200
#define JOY_DEAD_ZONE     20      // margen alrededor del centro (128)
#define JOY_CENTER  120   // ← corregido
// Prototipos
static bool controller_read(int *jx, bool *fire);
static bool serial_read_line(char *buf, int max_len);
// Variables compartidas
static int  g_jx   = JOY_CENTER;
static bool g_fire = false;
static HANDLE hThread = NULL;
static bool ismove = false;

static HANDLE hSerial = INVALID_HANDLE_VALUE;
static bool   controller_ready = false;
// Hilo de lectura
static DWORD WINAPI controller_thread(LPVOID param) {
    int  jx;
    bool fire;
    while (controller_ready) {
        if (controller_read(&jx, &fire)) {
            g_jx   = jx;
            g_fire = fire;
        }
        //Sleep(0.5);
    }
    return 0;
}

// ── Inicializar puerto serial ─────────────────────────────────
void controller_init(void) {
    char full_port[32];
    snprintf(full_port, sizeof(full_port), "\\\\.\\%s", CONTROLLER_PORT);

    hSerial = CreateFileA(full_port, GENERIC_READ, 0, NULL,
                          OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hSerial == INVALID_HANDLE_VALUE) {
        printf("[Controller] No se pudo abrir %s, usando solo teclado.\n", CONTROLLER_PORT);
        return;
    }

    DCB dcb = { 0 };
    dcb.DCBlength = sizeof(DCB);
    GetCommState(hSerial, &dcb);
    dcb.BaudRate = CONTROLLER_BAUD;
    dcb.ByteSize = 8;
    dcb.StopBits = ONESTOPBIT;
    dcb.Parity   = NOPARITY;
    SetCommState(hSerial, &dcb);

  

// En controller_init(), cambia los timeouts:
COMMTIMEOUTS timeouts = { 0 };
timeouts.ReadIntervalTimeout        = 50;   // espera hasta 50ms entre bytes
timeouts.ReadTotalTimeoutMultiplier = 0;
timeouts.ReadTotalTimeoutConstant   = 200;  // timeout total de 200ms por lectura
SetCommTimeouts(hSerial, &timeouts);
    controller_ready = true;
    printf("[Controller] Control conectado en %s\n", CONTROLLER_PORT);
    printf("[Controller] Handle valido: %s\n", hSerial != INVALID_HANDLE_VALUE ? "SI" : "NO"); 
    hThread = CreateThread(NULL, 0, controller_thread, NULL, 0, NULL);
}


void controller_close(void) {
    if (hSerial != INVALID_HANDLE_VALUE) {
        CloseHandle(hSerial);
        hSerial = INVALID_HANDLE_VALUE;
        controller_ready = false;
        if (hThread) {
            WaitForSingleObject(hThread, 1000);
            CloseHandle(hThread);
        }
    }
}

static bool serial_read_line(char *buf, int max_len) {
    int  i = 0;
    char c;
    DWORD read_count;

    while (i < max_len - 1) {
        BOOL ok = ReadFile(hSerial, &c, 1, &read_count, NULL);
        if (!ok) {
            printf("[Serial] ReadFile error: %lu\n", GetLastError());
            return false;
        }
        if (read_count == 0) {
            printf("[Serial] ReadFile timeout (0 bytes)\n");
            return false;
        }
        if (c == '\n') break;
        if (c != '\r') buf[i++] = c;
    }
    buf[i] = '\0';
    return i > 0;
}
static bool controller_read(int *jx, bool *fire) {
    if (!controller_ready) return false;

    char line[64];
    if (!serial_read_line(line, sizeof(line))) {
        printf("[Controller] serial_read_line fallo (sin datos o timeout)\n");
        return false;
    }

    //printf("[Controller] Linea recibida: '%s'\n", line);  // ← ver qué llega exactamente

    int btn = 0, chk = 0;
    if (sscanf(line, "JX: %d | BTN: %d | CHK: %d", jx, &btn, &chk) != 3) {
        printf("[Controller] sscanf falló en parsear\n");
        return false;
    }

    *fire = (btn == 1);
    return true;
}

// ── Handle de input (teclado + control) ──────────────────────
void input_handle(SOCKET sock, UIEvent *role, GameState *game) {
    if (*role == EVENT_NONE) return;

    // ── Teclado (sin cambios) ─────────────────────────────────
    if (IsKeyPressed(KEY_SPACE)) {
        network_send_shoot(sock);
    }
    if (IsKeyPressed(KEY_RIGHT)) {
        network_send_right(sock);
    }
    if (IsKeyPressed(KEY_LEFT)) {
        network_send_left(sock);
    }
    
    // ── Control físico ────────────────────────────────────────
    if (g_jx < 20) {
        
        network_send_right(sock);
        printf("Me moví a la derecha\n");
        Sleep(game->speed);
            
        
        
    } else if (g_jx > 200) {
        network_send_left(sock);
        printf("Me moví a la izquierda\n");
        Sleep(game->speed);
    }
    if (g_fire) {
        printf("[Controller] DISPARANDO!\n");
        network_send_shoot(sock);
        g_fire = false; // resetear para no disparar infinito
        Sleep(game->speed);
    }

}