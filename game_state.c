#include "include/game_state.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "include/cJSON.h"
#include "include/structs.h"


// ================= FREE LIST =================
void free_alien_list(AlienNode* head) {
    while (head) {
        AlienNode* tmp = head;
        head = head->next;
        free(tmp);
    }
}

//================================================
// ================= PARSER JSON =================
//================================================
void process_message(const char* raw_json, GameState* state, AppState *UI) {

    //Convierte el texto en un árbol de datos CJSON
    cJSON* root = cJSON_Parse(raw_json);
    //Mensaje de error en caso de no parsear correctamente
    if (!root)
    {
        printf("\nERROR AL PARSEAR JSON:\n");
        printf("%s\n", cJSON_GetErrorPtr());
        return;
    }
    //Busca "type" en el JSON 
    cJSON* type = cJSON_GetObjectItem(root, "type");

    //Validar que "type" exista y que sea una cadena
    if (!type || !type->valuestring) {
        cJSON_Delete(root);
        return;
    }

    // Procesar el mensaje de JSON de "GAME_STATE"
    if (strcmp(type->valuestring, "GAME_STATE") == 0) {

        // ---------------- PLAYER ----------------
        //Busca el objeto "player"
        cJSON* player = cJSON_GetObjectItem(root, "jugador");
        
        if (player) {
            //Busca y copia el ID del jugador a los structs (MAX 9 caracteres)
            cJSON* id = cJSON_GetObjectItem(player, "jugador_id");
            if (id && id->valuestring)
                strncpy(state->player.player_id, id->valuestring, 9);
                
            //Busca y designa la ubicación del cañon del jugador en valores numéricos a los structs
            cJSON* cannon = cJSON_GetObjectItem(player, "cannon_x");
            if (cannon) state->player.cannon_x = cannon->valueint;

            cJSON* cannon_y = cJSON_GetObjectItem(player, "cannon_y");
            if (cannon_y) state->player.cannon_y = cannon_y->valueint;
            
            //Busca y designa la cantidad de vidas del jugador en valores numéricos a los structs
            cJSON* lives = cJSON_GetObjectItem(player, "vidas");
            if (lives) state->player.lives = lives->valueint;

            //Busca y designa el puntaje del jugador en valores numéricos a los structs
            cJSON* score = cJSON_GetObjectItem(player, "score");
            if (score) state->player.score = score->valueint;
        }
        


        // ---------------- ALIENS (LISTA ENLAZADA) ----------------

        //Busca el objeto "aliens"
        cJSON* aliens = cJSON_GetObjectItem(root, "aliens");

        //Borra la lista anterior. Liberar nodos de listas anteriores (Quita acumulación de memoria)
        free_alien_list(state->aliens);
        state->aliens = NULL;
    
        if (aliens) {
            cJSON* alien;

            //Recorre array de aliens
            cJSON_ArrayForEach(alien, aliens) {

                //Reservar memoria a un alien del array
                AlienNode* node = malloc(sizeof(AlienNode));
                
                cJSON* id = cJSON_GetObjectItem(alien, "id");
                cJSON* x  = cJSON_GetObjectItem(alien, "x");
                cJSON* y  = cJSON_GetObjectItem(alien, "y");
                cJSON* pts = cJSON_GetObjectItem(alien, "points");
                
                //Designa todos los atributos a los structs, da 0 si están vacíos
                node->id  = id ? id->valueint : 0;
                node->x   = x  ? x->valueint  : 0;
                node->y   = y  ? y->valueint  : 0;
                node->pts = pts? pts->valueint: 0;
                
                //Verifica el tipo de alien
                cJSON* t = cJSON_GetObjectItem(alien, "type");
                if (t && t->valuestring)
                {
                    if (strcmp(t->valuestring, "cangrejo") == 0)
                        node->alienType = ALIEN_CRAB;

                    else if (strcmp(t->valuestring, "pulpo") == 0)
                        node->alienType = ALIEN_OCTOPUS;

                    else if (strcmp(t->valuestring, "calamar") == 0)
                        node->alienType = ALIEN_SQUID;

                    else
                        node->alienType = ALIEN_UNKNOWN;
                }

                //Inserta alien en el array (Inserta al inicio de lista)
                node->next = state->aliens;
                state->aliens = node;
            }
        }

        // ---------------- BULLETS ----------------
        //Busca el objeto "bullets"
        cJSON* bullets = cJSON_GetObjectItem(root, "balas");
        //Contador de balas 
        state->bullet_count = 0;

        if (bullets) {
            cJSON* b;
            
            //Recorre array de balas
            cJSON_ArrayForEach(b, bullets) {
                //Termina el recorrido una vez que se alcanza el limite
                if (state->bullet_count >= 100) break;

                //Designa todos los atributos a los structs (id,x,y)
                cJSON* id = cJSON_GetObjectItem(b, "id");
                cJSON* x = cJSON_GetObjectItem(b, "x");
                cJSON* y = cJSON_GetObjectItem(b, "y");
                

                if (!cJSON_IsNumber(x) || !cJSON_IsNumber(y))
                    continue;

                int idx = state->bullet_count;
                
                state->bullets[idx].id = id->valueint;
                state->bullets[idx].x = x->valueint;
                state->bullets[idx].y = y->valueint;

                
                state->bullets[idx].owner[0] = '\0';

                //Designa todos los atributos a los structs (owner)
                cJSON* owner = cJSON_GetObjectItem(b, "owner");
                

                if (cJSON_IsString(owner) && owner->valuestring)
                {
                    strncpy(state->bullets[idx].owner,owner->valuestring,
                        sizeof(state->bullets[idx].owner) - 1);

                    state->bullets[idx].owner[sizeof(state->bullets[idx].owner) - 1] = '\0';
                }
                //Incrementa el contador 
                state->bullet_count++;
            }
        }

        // ---------------- BUNKERS ----------------
        //Busca el objeto "bunkers"
        cJSON* bunkers = cJSON_GetObjectItem(root, "bunkers");
        //Contador de bunkers
        state->bunker_count = 0;

        if (bunkers) {
            cJSON* b;
            cJSON_ArrayForEach(b, bunkers) {
                //Termina el recorrido una vez que se alcanza el limite
                if (state->bunker_count >= 4) break;
                
                //Designa todos los atributos a los structs
                state->bunkers[state->bunker_count].id = cJSON_GetObjectItem(b,"id")->valueint;
                state->bunkers[state->bunker_count].x = cJSON_GetObjectItem(b, "x")->valueint;
                state->bunkers[state->bunker_count].y = cJSON_GetObjectItem(b, "y")->valueint;
                state->bunkers[state->bunker_count].health = cJSON_GetObjectItem(b, "health")->valueint;

                //Incrementa el contador 
                state->bunker_count++;
            }
        }

        // ---------------- UFO ----------------
        // Reiniciar UFO
        state->ufo.exists = false;
        state->ufo.active = false;
        state->ufo.x = 0;
        state->ufo.y = 0;
        state->ufo.points = 0;

        //Busca el objeto "ufo"
        cJSON* ufo = cJSON_GetObjectItem(root, "ufo");

        //Verificar si el ufo está vacío
        if (ufo && cJSON_IsObject(ufo) && ufo->child != NULL)
        {
            state->ufo.exists = true;

            cJSON* activo = cJSON_GetObjectItem(ufo, "activo");
            cJSON* x      = cJSON_GetObjectItem(ufo, "x");
            cJSON* y      = cJSON_GetObjectItem(ufo, "y");
            cJSON* points = cJSON_GetObjectItem(ufo, "points");

            if (activo && x && y && points)
            {
                state->ufo.active = activo->valueint;
                state->ufo.x      = x->valueint;
                state->ufo.y      = y->valueint;
                state->ufo.points = points->valueint;
            }
        }
        // ---------------- GAME META ----------------
        //Busca el objeto "wave"
        cJSON* wave = cJSON_GetObjectItem(root, "wave");
        //Designa el valor númerico de la oleada a los structs
        if (wave) state->wave = wave->valueint;

        //Busca el objeto "game_status"
        cJSON* status = cJSON_GetObjectItem(root, "game_status");
        //Designa el string de estado del juego a los structs
        if (status && status->valuestring)
            strncpy(state->game_status, status->valuestring, 19);
         //Busca el objeto "wave"
        cJSON* speed = cJSON_GetObjectItem(root, "alien_speed");
        //Designa el valor númerico de la oleada a los structs
        if (speed) state->speed = speed->valueint;
    }
    // Procesar el mensaje de JSON de "GAME_OVER"
    if (strcmp(type->valuestring, "GAME_OVER") == 0) {

        //Busca el objeto "reason"
        cJSON* reason = cJSON_GetObjectItem(root, "reason");
        //Designa el string de razon de perdida a los structs
        if (reason && reason->valuestring)
            strncpy(state->gameOver.reason, reason->valuestring, 30);

        //Busca el objeto "final_score"
        cJSON* final_score = cJSON_GetObjectItem(root, "final_score");
        //Designa el valor númerico de la oleada a los structs
        if (final_score) state->gameOver.final_score = final_score->valueint;
        
        // Cambiar estado de la pantalla
        *UI = GAME_OVER;

    }

    // Procesar mensaje de JSON de "ERROR"
    if (strcmp(type->valuestring, "ERROR") == 0) {
        //Busca el objeto "message"
        cJSON* message = cJSON_GetObjectItem(root, "message");
        //Designa el string de razon de perdida a los structs
        if (message && message->valuestring)
            strncpy(state->wrongID.message, message->valuestring, 30);
    
    }

    //Libera el árbol JSON (Liberar memoria reservada)
    cJSON_Delete(root);
}