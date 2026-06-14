#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

// ── Configuración ────────────────────────────────────────────
#define SEND_HZ     60
#define JOY_X_PIN   27      // GPIO27 → ADC canal 1
#define BTN_PIN     14
#define INTERVAL_MS (1000 / SEND_HZ)

// ── Funciones ────────────────────────────────────────────────

// Convierte lectura ADC 12-bit (0-4095) a byte (0-255)
static inline uint8_t read_axis(void) {
    uint16_t raw = adc_read();
    return (uint8_t)(raw >> 4);
}

// Envía el paquete como texto plano por USB
// Formato: "JX: 128 | BTN: 0 | CHK: 128"
static void send_packet(uint8_t jx, bool fire) {
    uint8_t btn = fire ? 1 : 0;
    uint8_t chk = (jx + btn) & 0xFF;
    printf("JX: %3d | BTN: %d | CHK: %3d\n", jx, btn, chk);
}

// ── Main ─────────────────────────────────────────────────────
int main(void) {
    stdio_init_all();

    // Esperar a que el USB esté listo
    sleep_ms(2000);

    // Inicializar ADC (GPIO27 → canal 1)
    adc_init();
    adc_gpio_init(JOY_X_PIN);
    adc_select_input(1);

    // Inicializar botón con pull-down interno
    gpio_init(BTN_PIN);
    gpio_set_dir(BTN_PIN, GPIO_IN);
    gpio_pull_down(BTN_PIN);

    printf("Control remoto iniciado — %d Hz\n", SEND_HZ);

    while (true) {
        uint32_t t0 = to_ms_since_boot(get_absolute_time());

        uint8_t jx   = read_axis();
        bool    fire = gpio_get(BTN_PIN);

        send_packet(jx, fire);

        uint32_t elapsed = to_ms_since_boot(get_absolute_time()) - t0;
        int32_t  wait    = (int32_t)INTERVAL_MS - (int32_t)elapsed;
        if (wait > 0) {
            sleep_ms((uint32_t)wait);
        }
    }

    return 0;
}