#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <windows.h>

// ── Configuración ─────────────────────────────────────────────
#define PUERTO    "COM7"
#define BAUD_RATE 115200

// ── Serial ────────────────────────────────────────────────────
static HANDLE hSerial = INVALID_HANDLE_VALUE;

bool serial_open(const char *port) {
    char full_port[32];
    snprintf(full_port, sizeof(full_port), "\\\\.\\%s", port);

    hSerial = CreateFileA(full_port, GENERIC_READ, 0, NULL,
                          OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hSerial == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "ERROR: No se pudo abrir %s (error %lu)\n",
                port, GetLastError());
        return false;
    }

    DCB dcb = { 0 };
    dcb.DCBlength = sizeof(DCB);
    GetCommState(hSerial, &dcb);
    dcb.BaudRate = BAUD_RATE;
    dcb.ByteSize = 8;
    dcb.StopBits = ONESTOPBIT;
    dcb.Parity   = NOPARITY;
    SetCommState(hSerial, &dcb);

    // Timeout: espera hasta 100ms por datos
    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout        = 100;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.ReadTotalTimeoutConstant   = 100;
    SetCommTimeouts(hSerial, &timeouts);

    return true;
}

// Lee una línea completa terminada en '\n'
bool serial_read_line(char *buf, int max_len) {
    int  i = 0;
    char c;
    DWORD read_count;

    while (i < max_len - 1) {
        if (!ReadFile(hSerial, &c, 1, &read_count, NULL) || read_count == 0)
            return false;
        if (c == '\n') break;
        if (c != '\r') buf[i++] = c;
    }
    buf[i] = '\0';
    return i > 0;
}

// ── Main ──────────────────────────────────────────────────────
int main(void) {
    printf("=== Test Control SpaCEinvaders ===\n");
    printf("Abriendo puerto %s...\n\n", PUERTO);

    if (!serial_open(PUERTO)) {
        printf("Verifica que la Pico W este conectada en %s.\n", PUERTO);
        return 1;
    }

    printf("Puerto abierto. Mueve el joystick y presiona el boton...\n");
    printf("Presiona Ctrl+C para salir.\n\n");
    printf("%-20s %s\n", "JX (0-255)", "FIRE");
    printf("-------------------------------\n");

    char line[64];
    int  jx = 128, btn = 0, chk = 0;
    int  prev_jx = -1, prev_btn = -1;

    while (1) {
        if (!serial_read_line(line, sizeof(line))) continue;

        // Parsear formato: "JX: 128 | BTN: 0 | CHK: 128"
        if (sscanf(line, "JX: %d | BTN: %d | CHK: %d", &jx, &btn, &chk) != 3)
            continue;

        // Solo imprimir si algo cambió
        if (jx == prev_jx && btn == prev_btn) continue;

        // Barra visual del joystick (20 caracteres)
        int pos = jx * 20 / 255;
        char bar[22] = {0};
        for (int i = 0; i < 20; i++)
            bar[i] = (i == pos) ? '|' : '-';

        printf("JX: %3d  [%s]  FIRE: %s\n",
            jx, bar, btn ? "PRESIONADO" : "          ");

        prev_jx  = jx;
        prev_btn = btn;
    }

    CloseHandle(hSerial);
    return 0;
}