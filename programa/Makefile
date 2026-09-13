CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Iinclude -Iinclude/cjson
SRC = src/main.c src/menu.c src/libro.c src/ejemplar.c src/usuario.c src/prestamo.c \
      src/persistencia.c src/fecha.c src/busqueda.c src/estadisticas.c src/cjson/cJSON.c
OBJ = $(SRC:.c=.o)
BIN = biblioteca

.PHONY: all clean run

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(OBJ) -o $(BIN) -lm

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	./$(BIN)

clean:
	rm -f $(OBJ) $(BIN)
