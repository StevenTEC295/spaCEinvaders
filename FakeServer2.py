import socket
import json
import time
import os
import struct


"""
# =========================================
# CONFIGURACIÓN
# =========================================

HOST = "127.0.0.1"
PORT = 8080

FILES_FOLDER = "Texto"

SEND_INTERVAL = 2  # segundos entre archivos

# =========================================
# SOCKET SERVER
# =========================================

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

server.bind((HOST, PORT))
server.listen(1)

# Espera al cliente
print("Esperando cliente C...")

# Acepta al cliente 
conn, addr = server.accept()
print(f"Cliente conectado: {addr}")

# leer el JOIN del cliente
data = conn.recv(1024).decode()
print(f"Recibido: {data}")

# =========================================
# FUNCIONES
# =========================================

def get_txt_files():

    #============================
    #Obtiene todos los archivos txt
    #============================
    

    if not os.path.exists(FILES_FOLDER):
        os.makedirs(FILES_FOLDER)

    files = [
        f for f in os.listdir(FILES_FOLDER)
        if f.endswith(".txt")
    ]

    return files


# =========================================
# Conexión del usuario dependiendo del rol
# =========================================

try:

    data = conn.recv(1024).decode().strip()

    print(f"Cliente dice: {data}")

    ack = {"type": "JOIN_ACK", "role": "PLAYER", "player_id": "P1", "cannon_x": 400}

    conn.sendall(ack)

except Exception as e:

    print("Error connection:", e)
    conn.close()
    server.close()
    exit()

# =========================================
# LOOP DE ARCHIVOS
# =========================================

while True:

    try:

        txt_files = get_txt_files()

        for filename in txt_files:
            path = os.path.join(FILES_FOLDER, filename)

            with open(path, "r", encoding="utf-8") as f:
                content = f.read()

            # Formato: nombre|contenido
            message = f"{filename}|{content}".encode("utf-8")

            # Enviar tamaño primero (4 bytes)
            conn.sendall(struct.pack("!I", len(message)))

            # Enviar mensaje completo
            conn.sendall(message)

            print(f"[SERVIDOR] Enviado: {filename}")

            #conn.setblocking(True)

            time.sleep(SEND_INTERVAL)

    except (BrokenPipeError, ConnectionResetError):

        print("Cliente desconectado")

    except KeyboardInterrupt:

        print("Servidor detenido manualmente")

    finally:

        conn.close()
        server.close()


"""
import socket
import struct
import os
import time

HOST = "127.0.0.1"
PORT = 8080

TXT_FOLDER = "Texto"

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind((HOST, PORT))
server.listen(1)

print("Servidor escuchando...")

conn, addr = server.accept()
print("Cliente conectado:", addr)

while True:
    files = [f for f in os.listdir(TXT_FOLDER) if f.endswith(".txt")]

    for f in files:
        path = os.path.join(TXT_FOLDER, f)

        with open(path, "r", encoding="utf-8") as file:
            content = file.read()

        # Formato simple: filename|content
        message = f"{f}|{content}".encode("utf-8")

        # Enviar tamaño + datos
        conn.sendall(struct.pack("!I", len(message)))
        conn.sendall(message)

        print("Enviado:", f)

        time.sleep(2)