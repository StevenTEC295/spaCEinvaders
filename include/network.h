#ifndef NETWORK_H
#define NETWORK_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include "structs.h"

SOCKET connect_server(const char *ip, int port);
//void network_send_join(SOCKET sock, int role);
//void network_send_input(SOCKET sock);

#endif
