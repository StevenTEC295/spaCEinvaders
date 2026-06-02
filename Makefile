PROJECT_NAME = game

CC = gcc

SRC_DIR = src
OBJ_DIR = obj

# Buscar todos los .c
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRC))

# Raylib (ajusta si lo tienes instalado en otro lado)
RAYLIB_PATH = C:/raylib
INCLUDE_PATHS = -I$(SRC_DIR) -I$(RAYLIB_PATH)/include -Iexternal/cJSON

# Librerías
LIBS = -lraylib -lopengl32 -lgdi32 -lwinmm

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

$(PROJECT_NAME): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) $(LIBS)

# Compilar objetos
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -rf $(OBJ_DIR) $(PROJECT_NAME).exe $(PROJECT_NAME)
    