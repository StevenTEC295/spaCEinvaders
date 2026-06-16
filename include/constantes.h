#ifndef CONSTANTES_H
#define CONSTANTES_H

#include "structs.h"

//Constantes de conexión y crear pantalla
//"127.0.0.1" prueba JAVA "192.168.100.56"
#define SERVER_IP "172.20.10.2"
//#define SERVER_IP "192.168.1.155"
#define SERVER_PORT 8080
#define BUFFER_SIZE 10000
#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 900

//Constantes de renderizado=========================
//BUNKERS
#define BUNKER_MAX_HP 100
#define BUNKER_STAGES 10
#define BUNKER_SPACING 60
//VIDAS
#define LIFE_ICON_SIZE_X 70
#define LIFE_ICON_SIZE_Y 40
//ENTRADA ID
#define PLAYER_ID_MAX_LENGTH 28
#define ASCII_MIN 32
#define ASCII_MAX 125
//TEXTO FIN DE JUEGO
#define FONT_SIZE_GAMEOVER 40
//TEXTO MEDIO EN PANTALLA
#define FONT_SIZE_MEDIUM 30
//TEXTO REGULARES EN PANTALLA
#define FONT_SIZE_NORMAL 20
//BOTONES Y ENTRADA
#define BUTTON_WIDTH 200
#define BUTTON_HEIGHT 60
#define INPUT_WIDTH 300
#define INPUT_HEIGHT 50
#define MENU_BUTTON_OFFSET_Y 70
#define MENU_BUTTON_SPACING 100

//RED=============================================
#define NETWORK_MSG_BUFFER_SIZE 128
#define WINSOCK_VERSION_MAJOR 2
#define WINSOCK_VERSION_MINOR 2
// ===== JSON INPUTS/JOIN =====
#define MSG_TYPE_JOIN   "JOIN"
#define MSG_TYPE_SHOOT  "SHOOT"
#define MSG_TYPE_MOVE   "MOVE"
// ===== MOVE DIRECTIONS =====
#define DIR_LEFT  "LEFT"
#define DIR_RIGHT "RIGHT"

// ===== ROLES =====
#define ROLE_PLAYER     "JUGADOR"
#define ROLE_SPECTATOR  "SPECTATOR"

//CONTROLADOR=========================================
// ── Configuración del control ─────────────────────────────────
#define CONTROLLER_PORT   "COM8"
#define CONTROLLER_BAUD   115200
#define JOY_DEAD_ZONE     20      // margen alrededor del centro (128)
#define JOY_CENTER  120   // ← corregido
#define SERIAL_PORT_BUFFER_SIZE 32
#define SERIAL_LINE_BUFFER_SIZE 64
#define SERIAL_FORMAT "JX: %d | BTN: %d | CHK: %d"
#define JOY_LEFT_LIMIT   20
#define JOY_RIGHT_LIMIT  200

//CJSON=======================================

#define MSG_GAME_STATE "GAME_STATE"
#define MSG_GAME_OVER  "GAME_OVER"
#define MSG_ERROR      "ERROR"

#define JSON_PLAYER    "jugador"
#define JSON_ALIENS    "aliens"
#define JSON_BULLETS   "balas"
#define JSON_BUNKERS   "bunkers"
#define JSON_UFO       "ufo"

#define JSON_WAVE      "wave"
#define JSON_STATUS    "game_status"
#define JSON_SPEED     "alien_speed"

#define JSON_TYPE      "type"
#define JSON_ID        "jugador_id"
#define JSON_CANNON_X   "cannon_x"
#define JSON_CANNON_Y  "cannon_y"
#define JSON_LIFES       "vidas"
#define JSON_SCORE      "score"

#define JSON_ALIEN_ID       "id"
#define JSON_ALIEN_X        "x"
#define JSON_ALIEN_Y        "y"
#define JSON_ALIEN_POINTS   "points"

#define ALIEN_TYPE_CRAB    "cangrejo"
#define ALIEN_TYPE_OCTOPUS "pulpo"
#define ALIEN_TYPE_SQUID   "calamar"

#define JSON_BULLETS_ID       "id"
#define JSON_BULLETS_X        "x"
#define JSON_BULLETS_Y        "y"
#define JSON_BULLETS_OWNER  "owner"

//=============

#define JSON_BUNKERS_ID         "id"
#define JSON_BUNKERS_X          "x"
#define JSON_BUNKERS_Y          "y"
#define JSON_BUNKERS_HEALTH     "health"

#define JSON_UFO_ACTIVE         "activo"
#define JSON_UFO_X              "x"
#define JSON_UFO_Y              "y"
#define JSON_UFO_POINTS         "points"

#define JSON_GAMEOVER_REASON      "reason"
#define JSON_GAMEOVER_FINAL_SCORE "final_score"

#define JSON_ERROR_MSG           "message"
//=======================

#define MAX_BULLETS 100
#define MAX_BUNKERS 4

#define PLAYER_ID_MAX 9
#define GAME_STATUS_MAX 19
#define TEXT_SHORT_MAX 30


extern TimerAnimation WaveMessage;

#endif