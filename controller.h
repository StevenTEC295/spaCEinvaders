#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <stdint.h>
#include <stdbool.h>

// ── Estado del control ────────────────────────────────────────
typedef struct {
    uint8_t jx;     // Joystick X: 0-255 (centro ≈ 128)
    bool    fire;   // Botón disparo
} ControllerState;

// ── API pública ───────────────────────────────────────────────

// Abre el puerto serial (ej: "COM6" en Windows)
// Retorna true si se abrió correctamente
bool controller_open(const char *port);

// Lee el último paquete disponible (no bloquea)
// Retorna true si se recibió un paquete válido
bool controller_read(ControllerState *state);

// Cierra el puerto serial
void controller_close(void);

#endif // CONTROLLER_H