#ifndef NETWORK_H
#define NETWORK_H

#include <winsock2.h>
#include "structs.h"

void network_init(SOCKET* sock);
void network_send_join(SOCKET sock, int role);
void network_send_input(SOCKET sock);

#endif
