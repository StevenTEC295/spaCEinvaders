#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/uart.h"

// ── Configuración ────────────────────────────────────────────
#define UART_ID     uart0
#define BAUD_RATE   115200
#define SEND_HZ     60          // paquetes por segundo
#define HEADER      0xFF

#define UART_TX_PIN 0
#define UART_RX_PIN 1
#define JOY_X_PIN   27          // GPIO27 → ADC1
#define BTN_PIN     14

#define INTERVAL_MS (1000 / SEND_HZ)

// ── Funciones ────────────────────────────────────────────────

/**
 * Convierte lectura ADC 12-bit del SDK de Pico (0-4095) a byte (0-255).
 * Nota: en MicroPython read_u16() devuelve 0-65535 (16 bits),
 * el SDK de C devuelve 0-4095 (12 bits), por eso se escala con >> 4.
 */
static inline uint8_t read_axis(void) {
    uint16_t raw = adc_read();   // 0–4095
    return (uint8_t)(raw >> 4);  // 0–255
}

/**
 * Construye y envía el paquete por UART.
 * Formato: [0xFF][JX][BTN][CHK]
 *   CHK = (JX + BTN) & 0xFF
 */
static void send_packet(uint8_t jx, bool fire) {
    uint8_t btn = fire ? 1 : 0;
    uint8_t chk = (jx + btn) & 0xFF;
    uint8_t packet[4] = { HEADER, jx, btn, chk };
    uart_write_blocking(UART_ID, packet, sizeof(packet));
}

// ── Main ─────────────────────────────────────────────────────
int main(void) {
    stdio_init_all();

    // Inicializar UART0
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    // Inicializar ADC y seleccionar canal (GPIO27 → canal 1)
    adc_init();
    adc_gpio_init(JOY_X_PIN);
    adc_select_input(1);        // canal 1 corresponde a GPIO27

    // Inicializar botón con pull-down interno
    gpio_init(BTN_PIN);
    gpio_set_dir(BTN_PIN, GPIO_IN);
    gpio_pull_down(BTN_PIN);

    printf("Control remoto iniciado — %d baud, %d Hz\n", BAUD_RATE, SEND_HZ);

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