PROJECT_NAME = probar

CC = gcc

# Archivos fuente directamente en la raíz
SRC = probar.c network.c game_state.c renderer.c assets.c input.c cJSON.c

# Raylib
RAYLIB_PATH = C:/raylib/w64devkit
INCLUDE_PATHS = -I. -Iinclude -I$(RAYLIB_PATH)/include

# Librerías
LIBS = -static -lraylib -lopengl32 -lgdi32 -lwinmm

# Winsock SOLO en Windows
ifeq ($(OS),Windows_NT)
    LIBS += -lws2_32
endif

# Linux soporte básico
ifeq ($(shell uname),Linux)
    LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
endif

CFLAGS = -Wall -std=c99 -O2 $(INCLUDE_PATHS)

LDFLAGS = -L$(RAYLIB_PATH)/lib

# Build final
all: $(PROJECT_NAME)

$(PROJECT_NAME): $(SRC)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) $(LIBS)

clean:
	rm -f $(PROJECT_NAME).exe $(PROJECT_NAME) 	