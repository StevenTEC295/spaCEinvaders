import socket, json, time

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
server.bind(("127.0.0.1", 8080))
server.listen(1)

print("Esperando cliente C...")
conn, addr = server.accept()
print(f"Cliente conectado: {addr}")

# leer el JOIN del cliente
data = conn.recv(1024).decode()
print(f"Recibido: {data}")

# responder con JOIN_ACK
ack = {"type": "JOIN_ACK", "role": "PLAYER", "player_id": "P1", "cannon_x": 400}
conn.sendall((json.dumps(ack) + "\n").encode())

# mandar GAME_STATE cada 100ms
game_state = {
    "type": "GAME_STATE",
    "player": {"player_id": "P1", "cannon_x": 400, "lives": 3, "score": 0},
    "aliens": [
        {"id": 1, "x": 3, "y": 1, "type": "octopus", "points": 40},
        {"id": 2, "x": 5, "y": 1, "type": "octopus", "points": 40},
        {"id": 3, "x": 3, "y": 2, "type": "crab",    "points": 20},
        {"id": 4, "x": 5, "y": 2, "type": "squid",   "points": 10}
    ],
    "bullets":    [],
    "bunkers":    [
        {"id": 1, "x": 150, "health": 100},
        {"id": 2, "x": 300, "health": 100}
    ],
    "ufo":        {"active": False, "x": 0, "direction": "L-R", "points": 0},
    "wave":       1,
    "alien_speed": 120,
    "game_status": "RUNNING"
}


alien_x = 3
direction = 1



try:
    while True:
        
        # mover aliens para simular movimiento real
        alien_x += direction
        if alien_x >= 8 or alien_x <= 0:
            direction *= -1

        for alien in game_state["aliens"]:
            alien["x"] += direction

        # leer input del cliente (no bloqueante)
        conn.setblocking(False)
        try:
            msg = conn.recv(1024).decode().strip()
            if msg:
                print(f"Cliente envió: {msg}")
                # simular un disparo que mata un alien
                if "SHOOT" in msg and len(game_state["aliens"]) > 0:
                    killed = game_state["aliens"].pop(0)
                    event = {
                        "type":      "ALIEN_KILLED",
                        "alien_id":  killed["id"],
                        "points":    killed["points"],
                        "new_score": killed["points"]
                    }
                    conn.sendall((json.dumps(event) + "\n").encode())
        except BlockingIOError:
            pass
        conn.setblocking(True)
        

        # Envia los datos
        conn.sendall((json.dumps(game_state) + "\n").encode())
        time.sleep(0.1)

except (BrokenPipeError, ConnectionResetError):
    print("Cliente desconectado")

conn.close()
server.close()