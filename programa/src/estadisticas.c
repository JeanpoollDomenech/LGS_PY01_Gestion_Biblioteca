#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/estadisticas.h"

/*
 * acumulador generico clave->conteo/suma, usado internamente por los 4
 * reportes para no repetir la logica de buscar o crear y sumar en cada uno
 * Es un arreglo dinamico (malloc/realloc), nunca un arreglo de tamano fijo
 */
typedef struct {
    char   *clave;
    int     conteo;
    double  suma;
} Acumulador;

static char *duplicar_cadena(const char *origen) {
    if (origen == NULL) {
        return NULL;
    }
    size_t longitud = strlen(origen) + 1;
    char *copia = malloc(longitud);
    if (copia != NULL) {
        memcpy(copia, origen, longitud);
    }
    return copia;
}

static int buscar_indice_acumulador(const Acumulador *acumuladores, int cantidad, const char *clave) {
    for (int i = 0; i < cantidad; i++) {
        if (strcmp(acumuladores[i].clave, clave) == 0) {
            return i;
        }
    }
    return -1;
}

/* busca clave en el acumulador si no existe lo agrega (realloc) con conteo/suma en 0 */
static int obtener_o_crear_acumulador(Acumulador **acumuladores, int *cantidad, const char *clave) {
    int idx = buscar_indice_acumulador(*acumuladores, *cantidad, clave);
    if (idx != -1) {
        return idx;
    }

    Acumulador *nuevo = realloc(*acumuladores, (size_t)(*cantidad + 1) * sizeof(Acumulador));
    if (nuevo == NULL) {
        return -1;
    }
    *acumuladores = nuevo;

    idx = *cantidad;
    (*acumuladores)[idx].clave = duplicar_cadena(clave);
    (*acumuladores)[idx].conteo = 0;
    (*acumuladores)[idx].suma = 0.0;
    (*cantidad)++;

    return idx;
}

static void sumar_acumulador(Acumulador **acumuladores, int *cantidad,
                              const char *clave, double monto_a_sumar) {
    int idx = obtener_o_crear_acumulador(acumuladores, cantidad, clave);
    if (idx == -1) {
        return;
    }
    (*acumuladores)[idx].conteo += 1;
    (*acumuladores)[idx].suma += monto_a_sumar;
}

static void liberar_acumuladores(Acumulador *acumuladores, int cantidad) {
    for (int i = 0; i < cantidad; i++) {
        free(acumuladores[i].clave);
    }
    free(acumuladores);
}

static void ordenar_por_conteo_desc(Acumulador *acumuladores, int cantidad) {
    for (int i = 0; i < cantidad - 1; i++) {
        int idx_mayor = i;
        for (int j = i + 1; j < cantidad; j++) {
            if (acumuladores[j].conteo > acumuladores[idx_mayor].conteo) {
                idx_mayor = j;
            }
        }
        if (idx_mayor != i) {
            Acumulador temporal = acumuladores[i];
            acumuladores[i] = acumuladores[idx_mayor];
            acumuladores[idx_mayor] = temporal;
        }
    }
}

static void ordenar_por_suma_desc(Acumulador *acumuladores, int cantidad) {
    for (int i = 0; i < cantidad - 1; i++) {
        int idx_mayor = i;
        for (int j = i + 1; j < cantidad; j++) {
            if (acumuladores[j].suma > acumuladores[idx_mayor].suma) {
                idx_mayor = j;
            }
        }
        if (idx_mayor != i) {
            Acumulador temporal = acumuladores[i];
            acumuladores[i] = acumuladores[idx_mayor];
            acumuladores[idx_mayor] = temporal;
        }
    }
}

void estadistica_top_producciones(const Prestamo *prestamos, int cantidad_prestamos,
                                   const Ejemplar *ejemplares, int cantidad_ejemplares) {
    Acumulador *acumuladores = NULL;
    int cantidad_acumuladores = 0;

    for (int i = 0; i < cantidad_prestamos; i++) {
        for (int j = 0; j < prestamos[i].cantidad_ejemplares; j++) {
            int idx_ejemplar = buscar_ejemplar_por_id(ejemplares, cantidad_ejemplares,
                                                       prestamos[i].ids_ejemplares[j]);
            if (idx_ejemplar == -1) {
                continue; /* ejemplar no encontrado (dato inconsistente); se ignora */
            }
            sumar_acumulador(&acumuladores, &cantidad_acumuladores,
                              ejemplares[idx_ejemplar].nombre_libro, 0.0);
        }
    }

    printf("Top producciones mas prestadas:\n");
    if (cantidad_acumuladores == 0) {
        printf("  (sin prestamos registrados todavia)\n");
    } else {
        ordenar_por_conteo_desc(acumuladores, cantidad_acumuladores);
        int limite = cantidad_acumuladores < 3 ? cantidad_acumuladores : 3;
        for (int i = 0; i < limite; i++) {
            printf("  %d. %s (%d prestamos)\n", i + 1, acumuladores[i].clave, acumuladores[i].conteo);
        }
    }

    liberar_acumuladores(acumuladores, cantidad_acumuladores);
}

void estadistica_top_usuarios(const Prestamo *prestamos, int cantidad_prestamos) {
    Acumulador *acumuladores = NULL;
    int cantidad_acumuladores = 0;

    for (int i = 0; i < cantidad_prestamos; i++) {
        sumar_acumulador(&acumuladores, &cantidad_acumuladores, prestamos[i].usuario_id, 0.0);
    }

    printf("Top usuarios con mas prestamos:\n");
    if (cantidad_acumuladores == 0) {
        printf("  (sin prestamos registrados todavia)\n");
    } else {
        ordenar_por_conteo_desc(acumuladores, cantidad_acumuladores);
        int limite = cantidad_acumuladores < 3 ? cantidad_acumuladores : 3;
        for (int i = 0; i < limite; i++) {
            printf("  %d. %s (%d prestamos)\n", i + 1, acumuladores[i].clave, acumuladores[i].conteo);
        }
    }

    liberar_acumuladores(acumuladores, cantidad_acumuladores);
}

void estadistica_top_meses_recaudado(const Prestamo *prestamos, int cantidad_prestamos) {
    Acumulador *acumuladores = NULL;
    int cantidad_acumuladores = 0;

    for (int i = 0; i < cantidad_prestamos; i++) {
        if (prestamos[i].monto <= 0.0) {
            continue; /* solo prestamos ya cobrados devueltos/finalizados cuentan como recaudado */
        }
        if (prestamos[i].fecha_inicio == NULL || strlen(prestamos[i].fecha_inicio) < 7) {
            continue; /* fecha invalida o incompleta, se ignora */
        }

        char mes_anio[8]; /* "YYYY-MM" + '\0' */
        memcpy(mes_anio, prestamos[i].fecha_inicio, 7);
        mes_anio[7] = '\0';

        sumar_acumulador(&acumuladores, &cantidad_acumuladores, mes_anio, prestamos[i].monto);
    }

    printf("Top mes-anio con mayor monto recaudado:\n");
    if (cantidad_acumuladores == 0) {
        printf("  (sin prestamos finalizados con monto cobrado todavia)\n");
    } else {
        ordenar_por_suma_desc(acumuladores, cantidad_acumuladores);
        int limite = cantidad_acumuladores < 5 ? cantidad_acumuladores : 5;
        for (int i = 0; i < limite; i++) {
            printf("  %d. %s (colones %.2f)\n", i + 1, acumuladores[i].clave, acumuladores[i].suma);
        }
    }

    liberar_acumuladores(acumuladores, cantidad_acumuladores);
}

void estadistica_generos_extremos(const Prestamo *prestamos, int cantidad_prestamos,
                                   const Ejemplar *ejemplares, int cantidad_ejemplares,
                                   const Libro *catalogo, int cantidad_libros) {
    Acumulador *acumuladores = NULL;
    int cantidad_acumuladores = 0;

    for (int i = 0; i < cantidad_prestamos; i++) {
        for (int j = 0; j < prestamos[i].cantidad_ejemplares; j++) {
            int idx_ejemplar = buscar_ejemplar_por_id(ejemplares, cantidad_ejemplares,
                                                       prestamos[i].ids_ejemplares[j]);
            if (idx_ejemplar == -1) {
                continue;
            }

            int idx_libro = buscar_libro_por_nombre(catalogo, cantidad_libros,
                                                     ejemplares[idx_ejemplar].nombre_libro);
            if (idx_libro == -1) {
                continue;
            }

            sumar_acumulador(&acumuladores, &cantidad_acumuladores, catalogo[idx_libro].genero, 0.0);
        }
    }

    if (cantidad_acumuladores == 0) {
        printf("Generos con prestamos: (ninguno registrado todavia)\n");
        liberar_acumuladores(acumuladores, cantidad_acumuladores);
        return;
    }

    ordenar_por_conteo_desc(acumuladores, cantidad_acumuladores);

    printf("Genero con mas prestamos: %s (%d)\n", acumuladores[0].clave, acumuladores[0].conteo);
    printf("Genero con menos prestamos: %s (%d)\n",
           acumuladores[cantidad_acumuladores - 1].clave,
           acumuladores[cantidad_acumuladores - 1].conteo);

    liberar_acumuladores(acumuladores, cantidad_acumuladores);
}
