# fake_client.py
import socket, json, time, threading

HOST = "192.168.1.86"
PORT = 8080

client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client.connect((HOST, PORT))
print("Conectado al servidor Java")

def send(msg):
    client.sendall((json.dumps(msg) + "\n").encode())
    print(f"→ Enviado: {msg}")

def receive_loop():
    buffer = ""
    while True:
        try:
            data = client.recv(4096).decode()
            if not data:
                print("Servidor cerró la conexión")
                break
            buffer += data
            while "\n" in buffer:
                line, buffer = buffer.split("\n", 1)
                if line.strip():
                    msg = json.loads(line)
                    print(f"← Recibido: {msg['type']}")
                    if msg['type'] == "GAME_STATE":
                        print(msg)
        except Exception as e:
            print(f"Error en receive_loop: {e}")
            break

# hilo que escucha respuestas del servidor
t = threading.Thread(target=receive_loop, daemon=True)
t.start()

# simular flujo inicial
send({"type": "JOIN", "role": "JUGADOR", "jugador_id": "P2"})
time.sleep(0.5)

send({"type": "MOVE", "direction": "LEFT"})
time.sleep(0.1)
send({"type": "MOVE", "direction": "RIGHT"})
time.sleep(0.1)
send({"type": "SHOOT"})
time.sleep(1)

# ── mantener el script vivo ──────────────────────────────────
print("\nEscribe un comando y Enter para enviar:")
print("  l → MOVE LEFT")
print("  r → MOVE RIGHT")
print("  s → SHOOT")
print("  q → salir\n")

while True:
    try:
        cmd = input("> ").strip().lower()
        if cmd == "l":
            send({"type": "MOVE", "direction": "LEFT"})
        elif cmd == "r":
            send({"type": "MOVE", "direction": "RIGHT"})
        elif cmd == "s":
            send({"type": "SHOOT"})
        elif cmd == "q":
            break
        else:
            print("Comandos: l, r, s, q")
    except KeyboardInterrupt:
        break

print("Cerrando...")
client.close()