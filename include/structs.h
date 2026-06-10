#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdbool.h>
// ---------------- PLAYER ----------------
typedef struct {
    char player_id[10], role[10];
    int cannon_x, cannon_y, lives, score;
} Player;

// ---------------- SPECTATOR ----------------
typedef struct {
    char watch_player[10], role[10];
} Spectator;

// ---------------- ALIEN (LISTA ENLAZADA) ----------------
typedef enum //Tipos de aliens
{
    ALIEN_CRAB,
    ALIEN_OCTOPUS,
    ALIEN_SQUID,
    ALIEN_UNKNOWN
} AlienType;

typedef struct AlienNode {
    int id, x, y, pts;
    AlienType alienType;
    struct AlienNode* next;    // lista enlazada
} AlienNode;



// ---------------- BULLET ----------------
typedef struct {
    int id,x, y;
    //bool active;
    char owner[10];
} Bullet;

// ---------------- BUNKER ----------------
typedef struct {
    int id, x, y;
    int health;
} Bunker;

// ---------------- UFO ----------------
typedef struct {
    bool exists; //Revisa si aparece del todo
    bool active;
    int x, y, points;
} UFO;

// ---------------- GAME STATE ----------------
typedef struct {
    Player player;
    AlienNode* aliens;   // cabeza de la lista enlazada
    Bullet bullets[100];
    int bullet_count;
    Bunker bunkers[4];
    int bunker_count;
    UFO ufo;
    int wave;
    char game_status[20];

} GameState;

// -----------------Shared State(Mensajes de Server)----------------
typedef struct {
    char buffer[4096];
    int has_new_data;
} SharedState;

// ================= UI STATE =================
typedef enum {
    MENU,
    GAME_PLAYER,
    GAME_SPECTATOR
} AppState;

//================== UI STATE (Enviar Server)=========
typedef enum {
    EVENT_NONE,
    EVENT_JOIN_PLAYER,
    EVENT_JOIN_SPECTATOR
} UIEvent;


#endif

