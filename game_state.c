#include "game_state.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "cJSON.h"

void free_alien_list(AlienNode* head) {
    while (head) {
        AlienNode* tmp = head;
        head = head->next;
        free(tmp);
    }
}

void process_message(const char* raw_json, GameState* state) {

    cJSON* root = cJSON_Parse(raw_json);
    if (!root) return;

    cJSON* type = cJSON_GetObjectItem(root, "type");
    if (!type || !type->valuestring) {
        cJSON_Delete(root);
        return;
    }

    if (strcmp(type->valuestring, "GAME_STATE") == 0) {

        // PLAYER
        cJSON* player = cJSON_GetObjectItem(root, "jugador");
        if (player) {
            cJSON* id = cJSON_GetObjectItem(player, "jugador_id");
            if (id && id->valuestring)
                strncpy(state->player.player_id, id->valuestring, 9);

            cJSON* cannon = cJSON_GetObjectItem(player, "cannon_x");
            if (cannon) state->player.cannon_x = cannon->valueint;

            cJSON* lives = cJSON_GetObjectItem(player, "vidas");
            if (lives) state->player.lives = lives->valueint;

            cJSON* score = cJSON_GetObjectItem(player, "score");
            if (score) state->player.score = score->valueint;
        }

        // ALIENS
        cJSON* aliens = cJSON_GetObjectItem(root, "aliens");

        free_alien_list(state->aliens);
        state->aliens = NULL;

        if (aliens) {
            cJSON* alien;

            cJSON_ArrayForEach(alien, aliens) {
                AlienNode* node = malloc(sizeof(AlienNode));

                node->id = cJSON_GetObjectItem(alien, "id")->valueint;
                node->x  = cJSON_GetObjectItem(alien, "x")->valueint;
                node->y  = cJSON_GetObjectItem(alien, "y")->valueint;
                node->pts= cJSON_GetObjectItem(alien, "points")->valueint;

                node->next = state->aliens;
                state->aliens = node;
            }
        }

        // BULLETS (FIX importante)
        cJSON* bullets = cJSON_GetObjectItem(root, "balas");
        state->bullet_count = 0;

        if (bullets) {
            cJSON* b;
            cJSON_ArrayForEach(b, bullets) {

                if (state->bullet_count >= 100) break;

                state->bullets[state->bullet_count].x =
                    cJSON_GetObjectItem(b, "x")->valueint;

                state->bullets[state->bullet_count].y =
                    cJSON_GetObjectItem(b, "y")->valueint;

                cJSON* owner = cJSON_GetObjectItem(b, "owner");
                if (owner && owner->valuestring)
                    strncpy(state->bullets[state->bullet_count].owner,
                            owner->valuestring, 9);

                state->bullet_count++;
            }
        }

        // BUNKERS
        cJSON* bunkers = cJSON_GetObjectItem(root, "bunkers");
        state->bunker_count = 0;

        if (bunkers) {
            cJSON* b;
            cJSON_ArrayForEach(b, bunkers) {

                if (state->bunker_count >= 10) break;

                state->bunkers[state->bunker_count].id =
                    cJSON_GetObjectItem(b, "id")->valueint;

                state->bunkers[state->bunker_count].x =
                    cJSON_GetObjectItem(b, "x")->valueint;

                state->bunkers[state->bunker_count].health =
                    cJSON_GetObjectItem(b, "health")->valueint;

                state->bunker_count++;
            }
        }

        // UFO
        cJSON* ufo = cJSON_GetObjectItem(root, "ufo");
        if (ufo) {
            state->ufo.active = cJSON_GetObjectItem(ufo, "activo")->valueint;
            state->ufo.x      = cJSON_GetObjectItem(ufo, "x")->valueint;
            state->ufo.points = cJSON_GetObjectItem(ufo, "points")->valueint;
        }

        // META
        cJSON* wave = cJSON_GetObjectItem(root, "wave");
        if (wave) state->wave = wave->valueint;
    }

    cJSON_Delete(root);
}