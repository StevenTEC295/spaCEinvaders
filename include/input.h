#ifndef INPUT_H
#define INPUT_H

#include "structs.h"
#include <winsock2.h>

void input_handle(SOCKET sock, UIEvent *role);

#endif
