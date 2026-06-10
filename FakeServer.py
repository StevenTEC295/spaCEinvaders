import socket, json, time,random

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
'type': 'GAME_STATE',
'jugador': {'jugador_id': 'P2', 'cannon_x': 760, 'cannon_y': 817, 'vidas': 3, 'score': 0},
'aliens': [
	{'id': 1, 'x': 0, 'y': 2, 'type': 'pulpo', 'points': 40},
	{'id': 2, 'x': 1, 'y': 2, 'type': 'pulpo', 'points': 40}, 
    {'id': 3, 'x': 2, 'y': 2, 'type': 'pulpo', 'points': 40}, 
    {'id': 4, 'x': 3, 'y': 2, 'type': 'pulpo', 'points': 40}, 
    {'id': 5, 'x': 4, 'y': 2, 'type': 'pulpo', 'points': 40}, 
    {'id': 6, 'x': 5, 'y': 2, 'type': 'pulpo', 'points': 40}, 
    {'id': 7, 'x': 6, 'y': 2, 'type': 'pulpo', 'points': 40},
    {'id': 8, 'x': 7, 'y': 2, 'type': 'pulpo', 'points': 40}, 
    {'id': 9, 'x': 8, 'y': 2, 'type': 'pulpo', 'points': 40}, 
    {'id': 10, 'x': 9, 'y': 2, 'type': 'pulpo', 'points': 40}, 
    {'id': 11, 'x': 10, 'y': 2, 'type': 'pulpo', 'points': 40}, 
    {'id': 12, 'x': 0, 'y': 3, 'type': 'cangrejo', 'points': 20}, 
    {'id': 13, 'x': 1, 'y': 3, 'type': 'cangrejo', 'points': 20}, 
    {'id': 14, 'x': 2, 'y': 3, 'type': 'cangrejo', 'points': 20}, 
    {'id': 15, 'x': 3, 'y': 3, 'type': 'cangrejo', 'points': 20},
    {'id': 16, 'x': 4, 'y': 3, 'type': 'cangrejo', 'points': 20}, 
    {'id': 17, 'x': 5, 'y': 3, 'type': 'cangrejo', 'points': 20}, 
    {'id': 18, 'x': 6, 'y': 3, 'type': 'cangrejo', 'points': 20}, 
    {'id': 19, 'x': 7, 'y': 3, 'type': 'cangrejo', 'points': 20}, 
    {'id': 20, 'x': 8, 'y': 3, 'type': 'cangrejo', 'points': 20}, 
    {'id': 21, 'x': 9, 'y': 3, 'type': 'cangrejo', 'points': 20}, 
    {'id': 22, 'x': 10, 'y': 3, 'type': 'cangrejo', 'points': 20}, 
    {'id': 23, 'x': 0, 'y': 4, 'type': 'cangrejo', 'points': 20}, 
    {'id': 24, 'x': 1, 'y': 4, 'type': 'cangrejo', 'points': 20}, 
    {'id': 25, 'x': 2, 'y': 4, 'type': 'cangrejo', 'points': 20}, 
    {'id': 26, 'x': 3, 'y': 4, 'type': 'cangrejo', 'points': 20}, 
    {'id': 27, 'x': 4, 'y': 4, 'type': 'cangrejo', 'points': 20}, 
    {'id': 28, 'x': 5, 'y': 4, 'type': 'cangrejo', 'points': 20},
    {'id': 29, 'x': 6, 'y': 4, 'type': 'cangrejo', 'points': 20},
    {'id': 30, 'x': 7, 'y': 4, 'type': 'cangrejo', 'points': 20}, 
    {'id': 31, 'x': 8, 'y': 4, 'type': 'cangrejo', 'points': 20}, 
    {'id': 32, 'x': 9, 'y': 4, 'type': 'cangrejo', 'points': 20}, 
    {'id': 33, 'x': 10, 'y': 4, 'type': 'cangrejo', 'points': 20},
    {'id': 34, 'x': 0, 'y': 5, 'type': 'calamar', 'points': 10}, 
    {'id': 35, 'x': 1, 'y': 5, 'type': 'calamar', 'points': 10}, 
    {'id': 36, 'x': 2, 'y': 5, 'type': 'calamar', 'points': 10}, 
    {'id': 37, 'x': 3, 'y': 5, 'type': 'calamar', 'points': 10},
    {'id': 38, 'x': 4, 'y': 5, 'type': 'calamar', 'points': 10}, 
    {'id': 39, 'x': 5, 'y': 5, 'type': 'calamar', 'points': 10}, 
    {'id': 40, 'x': 6, 'y': 5, 'type': 'calamar', 'points': 10}, 
    {'id': 41, 'x': 7, 'y': 5, 'type': 'calamar', 'points': 10}, 
    {'id': 42, 'x': 8, 'y': 5, 'type': 'calamar', 'points': 10}, 
    {'id': 43, 'x': 9, 'y': 5, 'type': 'calamar', 'points': 10}, 
    {'id': 44, 'x': 10, 'y': 5, 'type': 'calamar', 'points': 10},
    {'id': 45, 'x': 0, 'y': 6, 'type': 'calamar', 'points': 10},
    {'id': 46, 'x': 1, 'y': 6, 'type': 'calamar', 'points': 10}, 
    {'id': 47, 'x': 2, 'y': 6, 'type': 'calamar', 'points': 10},
    {'id': 48, 'x': 3, 'y': 6, 'type': 'calamar', 'points': 10},
    {'id': 49, 'x': 4, 'y': 6, 'type': 'calamar', 'points': 10}, 
    {'id': 50, 'x': 5, 'y': 6, 'type': 'calamar', 'points': 10}, 
    {'id': 51, 'x': 6, 'y': 6, 'type': 'calamar', 'points': 10}, 
    {'id': 52, 'x': 7, 'y': 6, 'type': 'calamar', 'points': 10}, 
    {'id': 53, 'x': 8, 'y': 6, 'type': 'calamar', 'points': 10}, 
    {'id': 54, 'x': 9, 'y': 6, 'type': 'calamar', 'points': 10}, 
    {'id': 55, 'x': 10, 'y': 6, 'type': 'calamar', 'points': 10}], 
    'balas': [
        {'id': 1, 'x': 110, 'y': 372, 'owner': 'alien'}, 
        {'id': 2, 'x': 206, 'y': 346, 'owner': 'alien'}, 
        {'id': 3, 'x': 78, 'y': 256, 'owner': 'alien'}, 
        {'id': 4, 'x': 142, 'y': 198, 'owner': 'alien'}], 
    'ufo': {}, 
    'bunkers': [
        {'id': 1, 'x': 2, 'y': 12, 'health': 100},
        {'id': 2, 'x': 6, 'y': 12, 'health': 100}, 
        {'id': 3, 'x': 10, 'y': 12, 'health': 100}, 
        {'id': 4, 'x': 14, 'y': 12, 'health': 100}], 
        'wave': 1, 
        'alien_speed': 500, 
        'game_status': 'RUNNING'
}


SCREEN_WIDTH = 1200

SCREEN_HEIGHT = 800

PLAYER_WIDTH = 60
PLAYER_HEIGHT = 30

ALIEN_WIDTH = 55
ALIEN_HEIGHT = 75

BUNKER_WIDTH = 160
BUNKER_HEIGHT = 120

BULLET_WIDTH = 8
BULLET_HEIGHT = 20

PLAYER_BULLET_SPEED = -20
ALIEN_BULLET_SPEED = 10

player_bullets = []
bullet_id_counter = 1000

ALIEN_SHOOT_RATE = 0.03  # probabilidad por frame
player_shooting = False

direction = 5      # velocidad horizontal
drop_distance = 50  # cuánto bajan al tocar un borde

counter = 0
movement_ufo = 20

counter_bunker = 0

def collision(x1, y1, w1, h1, x2, y2, w2, h2):
    return (
        x1 < x2 + w2 and
        x1 + w1 > x2 and
        y1 < y2 + h2 and
        y1 + h1 > y2
    )

try:
    while True:
        counter += 1
        counter_bunker += 1
        current_bunker = 0
        
        #ALIENS==========================================================
        # Detectar extremos de la formación
        leftmost = min(alien["x"] for alien in game_state["aliens"])
        rightmost = max(alien["x"] for alien in game_state["aliens"])

        # ¿La formación tocó un borde?
        hit_border = (
            rightmost >= SCREEN_WIDTH - 100 or
            leftmost <= 0
        )

        if hit_border:
            direction *= -1

            # bajar toda la formación
            for alien in game_state["aliens"]:
                alien["y"] += drop_distance

        # mover toda la formación horizontalmente
        for alien in game_state["aliens"]:
            alien["x"] += direction

        #UFO==========================================================
        # activar/desactivar UFO cada cierto tiempo
        
        # Crear UFO cada 10 segundos si no existe
        if counter >= 10 and not game_state["ufo"]:
            game_state["ufo"] = {
                "activo": True,
                "x": 3,
                "points": 14
            }
            counter = 0

        # Mover UFO si existe
        if game_state["ufo"]:
            game_state["ufo"]["x"] += movement_ufo

            # Eliminar UFO cuando salga de la pantalla
            if game_state["ufo"]["x"] > SCREEN_WIDTH:
                game_state["ufo"] = {}
        
        # Vida de los bunkers====================================================
        # =========================
        # MOVE PLAYER BULLETS
        # =========================
        for bullet in game_state["balas"]:
            if bullet["owner"] == "jugador":
                bullet["y"] += PLAYER_BULLET_SPEED
            else:
                bullet["y"] += ALIEN_BULLET_SPEED

        #==========================
        # COLISIONES
        #==========================
        bullets_to_remove = []

        player = game_state["jugador"]

        for bullet in game_state["balas"]:

            # =========================
            # PLAYER BULLET → ALIENS
            # =========================
            if bullet["owner"] == "jugador":

                for alien in game_state["aliens"][:]:

                    if (
                        bullet["x"] < alien["x"] + 50 and
                        bullet["x"] + 4 > alien["x"] and
                        bullet["y"] < alien["y"] + 40 and
                        bullet["y"] + 10 > alien["y"]
                    ):
                        player["score"] += alien["points"]
                        game_state["aliens"].remove(alien)
                        bullets_to_remove.append(bullet)
                        player["vidas"] += 1
                        # se desactiva después del frame
                        player_shooting = False
                        break

            # =========================
            # BULLET → BORDER
            # =========================
            if bullet["y"] < 0 or bullet["y"] > SCREEN_HEIGHT:
                bullets_to_remove.append(bullet)

            # =========================
            # BULLET → BUNKERS
            # =========================
            for bunker in game_state["bunkers"][:]:

                if (
                    bullet["x"] <= bunker["x"] + 80 and
                    bullet["x"] >= bunker["x"] and
                    bullet["y"] <= bunker["y"] + 60 and
                    bullet["y"] >= bunker["y"]
                ):
                    bunker["health"] -= 10

                    if bunker["health"] <= 0:
                        game_state["bunkers"].remove(bunker)

                    bullets_to_remove.append(bullet)
                    # se desactiva después del frame
                    player_shooting = False
                    break

            # =========================
            # ALIEN BULLET → PLAYER
            # =========================
            if bullet["owner"] == "alien":

                if (
                    bullet["x"] <= player["cannon_x"] + 60 and
                    bullet["x"] + 7 >= player["cannon_x"] and
                    bullet["y"] <= 810 and
                    bullet["y"] + 7 >= 810
                ):
                    player["vidas"] -= 1
                    bullets_to_remove.append(bullet)

                    if player["vidas"] <= 0:
                        game_state["game_status"] = "GAME_OVER"

        #Remover balas
        for b in bullets_to_remove:
            if b in game_state["balas"]:
                game_state["balas"].remove(b)

        #Disparo automatico de aliens
        if game_state["aliens"]:

            if random.random() < ALIEN_SHOOT_RATE:

                shooter = random.choice(game_state["aliens"])

                game_state["balas"].append({
                    "id": bullet_id_counter,
                    "x": shooter["x"] + 20,
                    "y": shooter["y"] + 20,
                    "owner": "alien"
                })

                bullet_id_counter += 1

        # leer input del cliente (no bloqueante)
        conn.setblocking(False)
        try:
            msg = conn.recv(1024).decode().strip()
            if msg:
                print(f"Cliente envió: {msg}")
                # simular un disparo que mata un alien
                """
                if "SHOOT" in msg and len(game_state["aliens"]) > 0:

                    killed = game_state["aliens"].pop(0)
                    event = {
                        "type":      "ALIEN_KILLED",
                        "alien_id":  killed["id"],
                        "points":    killed["points"],
                        "new_score": killed["points"]
                    }
                conn.sendall((json.dumps(event) + "\n").encode())
                """

                if "SHOOT" in msg:
                    player_shooting = True

                    game_state["balas"].append({
                        "id": bullet_id_counter,
                        "x": game_state["jugador"]["cannon_x"],
                        "y": 700,
                        "owner": "jugador"
                    })

                    bullet_id_counter += 1

                if "RIGHT" in msg:
                    game_state["jugador"]["cannon_x"] += 10

                if "LEFT" in msg:
                    game_state["jugador"]["cannon_x"] -= 10

                    
        except BlockingIOError:
            pass
        conn.setblocking(True)
        

        # Envia los datos
        conn.sendall((json.dumps(game_state) + "\n").encode())
        time.sleep(0.5)

except (BrokenPipeError, ConnectionResetError):
    print("Cliente desconectado")

conn.close()
server.close()