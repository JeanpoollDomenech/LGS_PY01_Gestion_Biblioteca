#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/libro.h"

Libro *leer_catalogo(int *cantidad) {
    /* lunes 31 integrar cJSON para leer data/libros.json */
    *cantidad = 0;
    return NULL;
}

void guardar_catalogo(const Libro *catalogo, int cantidad) {
    (void)catalogo;
    (void)cantidad;
    /* lunes 31 integrar cJSON para escribir data/libros.json */
}

int buscar_libro_por_nombre(const Libro *catalogo, int cantidad, const char *nombre) {
    for (int i = 0; i < cantidad; i++) {
        if (strcmp(catalogo[i].nombre, nombre) == 0) {
            return i;
        }
    }
    return -1;
}

Libro *cargar_lote_desde_archivo(const char *ruta, int *cantidad_leidos) {
    (void)ruta;
    /* martes 1 parsear archivo delimitado por '#' */
    *cantidad_leidos = 0;
    return NULL;
}

void procesar_lote(Libro **catalogo, int *cantidad_catalogo,
                    const Libro *lote, int cantidad_lote) {
    (void)catalogo;
    (void)cantidad_catalogo;
    (void)lote;
    (void)cantidad_lote;
    /* martes 1 insertar nuevos + generar ejemplares + reporte de no procesados */
}

void mostrar_catalogo(const Libro *catalogo, int cantidad) {
    if (cantidad == 0) {
        printf("El catalogo esta vacio.\n");
        return;
    }
    for (int i = 0; i < cantidad; i++) {
        printf("- %s (%s, %d) | genero: %s | cantidad: %d\n",
               catalogo[i].nombre, catalogo[i].autor,
               catalogo[i].anio_publicacion, catalogo[i].genero,
               catalogo[i].cantidad);
    }
}

void liberar_catalogo(Libro *catalogo, int cantidad) {
    for (int i = 0; i < cantidad; i++) {
        free(catalogo[i].nombre);
        free(catalogo[i].autor);
        free(catalogo[i].genero);
        free(catalogo[i].resumen);
    }
    free(catalogo);
}
