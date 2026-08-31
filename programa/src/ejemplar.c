#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/ejemplar.h"

Ejemplar *leer_ejemplares(int *cantidad) {
    /* lunes 31 hay integrar cJSON para leer data/ejemplares.json */
    *cantidad = 0;
    return NULL;
}

void guardar_ejemplares(const Ejemplar *ejemplares, int cantidad) {
    (void)ejemplares;
    (void)cantidad;
    /* lunes 31 hay integrar cJSON para escribir data/ejemplares.json */
}

int siguiente_id_ejemplar(const Ejemplar *ejemplares, int cantidad) {
    int mayor = 0;
    for (int i = 0; i < cantidad; i++) {
        if (ejemplares[i].id > mayor) {
            mayor = ejemplares[i].id;
        }
    }
    return mayor + 1;
}

Ejemplar *generar_ejemplares(const char *nombre_libro, int cantidad_nueva, const Ejemplar *existentes, int cantidad_existentes) {
    (void)nombre_libro;
    (void)cantidad_nueva;
    (void)existentes;
    (void)cantidad_existentes;
    /* mar 1 sep generar ejemplares nuevos al procesar el lote de catalogo */
    return NULL;
}

int buscar_ejemplar_por_id(const Ejemplar *ejemplares, int cantidad, int id) {
    for (int i = 0; i < cantidad; i++) {
        if (ejemplares[i].id == id) {
            return i;
        }
    }
    return -1;
}

void liberar_ejemplares(Ejemplar *ejemplares, int cantidad) {
    for (int i = 0; i < cantidad; i++) {
        free(ejemplares[i].nombre_libro);
    }
    free(ejemplares);
}
