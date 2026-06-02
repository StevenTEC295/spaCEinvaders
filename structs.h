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
typedef struct AlienNode {
    int id, x, y, pts;
    char type[16];
    struct AlienNode* next;    // lista enlazada
} AlienNode;


// ---------------- BULLET ----------------
typedef struct {
    int x, y;
    bool active;
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

