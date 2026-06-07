#ifndef STRUCTS_H
#define STRUCTS_H
#include <stdbool.h>
// ---------------- PLAYER ----------------
typedef struct {
    char player_id[10], role[10];
    int cannon_x, lives, score;
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
    int x, y;
    //bool active;
    char owner[10];
} Bullet;

// ---------------- BUNKER ----------------
typedef struct {
    int id, x;
    int health;
} Bunker;

// ---------------- UFO ----------------
typedef struct {
    bool active;
    int x, points;
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

#endif

