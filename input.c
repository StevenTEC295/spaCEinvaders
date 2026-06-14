#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOUSER

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
#define JOY_DEAD_ZONE     15      // margen alrededor del centro (128)
#define JOY_CENTER        112

static HANDLE hSerial = INVALID_HANDLE_VALUE;
static bool   controller_ready = false;

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

   COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout        = MAXDWORD;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.ReadTotalTimeoutConstant   = 0;
    SetCommTimeouts(hSerial, &timeouts);

    controller_ready = true;
    printf("[Controller] Control conectado en %s\n", CONTROLLER_PORT);
    printf("[Controller] Handle valido: %s\n", hSerial != INVALID_HANDLE_VALUE ? "SI" : "NO"); 
}

void controller_close(void) {
    if (hSerial != INVALID_HANDLE_VALUE) {
        CloseHandle(hSerial);
        hSerial = INVALID_HANDLE_VALUE;
        controller_ready = false;
    }
}

// ── Leer línea del serial ─────────────────────────────────────
static bool serial_read_line(char *buf, int max_len) {
    int  i = 0;
    char c;
    DWORD read_count;

    while (i < max_len - 1) {
        if (!ReadFile(hSerial, &c, 1, &read_count, NULL) || read_count == 0)
            return false;
         printf("[Serial] char: %c\n", c);
        if (c == '\n') break;
        if (c != '\r') buf[i++] = c;
    }
    buf[i] = '\0';
    return i > 0;
}

// ── Leer estado del control ───────────────────────────────────
static bool controller_read(int *jx, bool *fire) {
    if (!controller_ready) return false;

    char line[64];
    int  btn = 0, chk = 0;

    if (!serial_read_line(line, sizeof(line))) return false;
    if (sscanf(line, "JX: %d | BTN: %d | CHK: %d", jx, &btn, &chk) != 3) return false;

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
    int  jx   = JOY_CENTER;
    bool fire = false;

    if (controller_read(&jx, &fire)) {
        printf("[Controller] JX: %d | FIRE: %d\n", jx, fire);
        // Joystick: valores bajos = derecha, valores altos = izquierda
        // (invertido por hardware, se interpreta al revés)
        if (jx < JOY_CENTER - JOY_DEAD_ZONE) {
            network_send_right(sock);   // valores bajos → derecha
        } else if (jx > JOY_CENTER + JOY_DEAD_ZONE) {
            network_send_left(sock);    // valores altos → izquierda
        }

        if (fire) {
            network_send_shoot(sock);
        }
    }
}