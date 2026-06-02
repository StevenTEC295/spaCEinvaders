from machine import Pin, ADC
import time

joyx = ADC(27)

btn_disparo = Pin(14, Pin.IN, Pin.PULL_DOWN)
def read_axis(adc):
    """Convierte lectura ADC 16-bit (0-65535) a byte (0-255)."""
    return adc.read_u16() >> 8
while(True):
    jx = read_axis(joyx)
    fire = btn_disparo.value()
    if fire == 1:
        print("Disparo")
    print(f"Movimiento X: {jx}")
    time.sleep(0.5)

