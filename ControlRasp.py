from machine import Pin, ADC, UART
import time
# ── Configuración ────────────────────────────────────────────
UART_ID   = 0
BAUD_RATE = 115200
SEND_HZ   = 60        # paquetes por segundo
HEADER    = 0xFF

uart     = UART(UART_ID, baudrate=BAUD_RATE, tx=Pin(0), rx=Pin(1))
joyx = ADC(27)

INTERVAL_MS = 1000 // SEND_HZ

btn_disparo = Pin(14, Pin.IN, Pin.PULL_DOWN)

def read_axis(adc):
    """Convierte lectura ADC 16-bit (0-65535) a byte (0-255)."""
    return adc.read_u16() >> 8
def build_packet(jx, fire):
    """
    Formato: [0xFF][JX][BTN][CHK]
      CHK = (JX + BTN) & 0xFF
    """
    btn = 1 if fire else 0
    chk = (jx + btn) & 0xFF
    return bytes([HEADER, jx, btn, chk])
 
print("Control remoto iniciado —", BAUD_RATE, "baud,", SEND_HZ, "Hz")

while(True):
    t0 = time.ticks_ms()
    jx = read_axis(joyx)
    fire = btn_disparo.value() == 1;
    
    uart.write(build_packet(jx, fire))
    
    elapsed = time.ticks.diff(time.ticks_ms(), t0)
    wait = INTERVAL_MS - elapsed
    if wait > 0:
        time.sleep_ms(wait)

