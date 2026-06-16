#ifndef NETWORK_H
#define NETWORK_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include "structs.h"

SOCKET connect_server(const char *ip, int port);
int network_send_join(SOCKET sock, UIEvent *role, const char *id_player);
void network_send_shoot(SOCKET sock);
void network_send_right(SOCKET sock);
void network_send_left(SOCKET sock);

#endif
