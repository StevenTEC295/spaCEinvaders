#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "structs.h"

void free_alien_list(AlienNode* head);
void process_message(const char* raw_json, GameState* state, AppState* UI);

#endif
