#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/busqueda.h"
#include "../include/estadisticas.h"

/* helper local solo para este harness de prueba strdup manual */
static char *dup_str(const char *s) {
    size_t len = strlen(s) + 1;
    char *copia = malloc(len);
    memcpy(copia, s, len);
    return copia;
}

int main(void) {
    /* ---------- catalogo de prueba (4 libros) ---------- */
    int cantidad_libros = 4;
    Libro catalogo[4];
    catalogo[0] = (Libro){ dup_str("Cien anios de soledad"), dup_str("Gabriel Garcia Marquez"), 1967, dup_str("Realismo magico"), dup_str("Historia de la familia Buendia"), 2 };
    catalogo[1] = (Libro){ dup_str("1984"), dup_str("George Orwell"), 1949, dup_str("Distopia"), dup_str("Un regimen totalitario vigila todo"), 2 };
    catalogo[2] = (Libro){ dup_str("Rayuela"), dup_str("Julio Cortazar"), 1963, dup_str("Realismo magico"), dup_str("Novela experimental de estructura abierta"), 1 };
    catalogo[3] = (Libro){ dup_str("Un mundo feliz"), dup_str("Aldous Huxley"), 1932, dup_str("Distopia"), dup_str("Sociedad futura controlada por el placer"), 1 };

    /* ---------- ejemplares de prueba (ids 1..6) ---------- */
    int cantidad_ejemplares = 6;
    Ejemplar ejemplares[6];
    ejemplares[0] = (Ejemplar){ 1, dup_str("Cien anios de soledad"), EJEMPLAR_DISPONIBLE };
    ejemplares[1] = (Ejemplar){ 2, dup_str("Cien anios de soledad"), EJEMPLAR_PRESTADO };
    ejemplares[2] = (Ejemplar){ 3, dup_str("1984"), EJEMPLAR_PRESTADO };
    ejemplares[3] = (Ejemplar){ 4, dup_str("1984"), EJEMPLAR_PRESTADO };
    ejemplares[4] = (Ejemplar){ 5, dup_str("Rayuela"), EJEMPLAR_PRESTADO };
    ejemplares[5] = (Ejemplar){ 6, dup_str("Un mundo feliz"), EJEMPLAR_DISPONIBLE };

    /* ---------- prestamos de prueba ---------- */
    int cantidad_prestamos = 4;
    Prestamo prestamos[4];

    int ids_p1[] = {2};
    prestamos[0] = (Prestamo){ 1, dup_str("111110000"), dup_str("2026-08-05"), dup_str("2026-08-12"), dup_str("2026-08-11"),
                                malloc(sizeof(int)), 1, PRESTAMO_FINALIZADO, 0, 175.0 * 6 };
    memcpy(prestamos[0].ids_ejemplares, ids_p1, sizeof(ids_p1));

    int ids_p2[] = {3, 4};
    prestamos[1] = (Prestamo){ 2, dup_str("222220000"), dup_str("2026-08-20"), dup_str("2026-08-27"), NULL,
                                malloc(sizeof(ids_p2)), 2, PRESTAMO_ACTIVO, 0, 0.0 };
    memcpy(prestamos[1].ids_ejemplares, ids_p2, sizeof(ids_p2));

    int ids_p3[] = {5};
    prestamos[2] = (Prestamo){ 3, dup_str("111110000"), dup_str("2026-09-01"), dup_str("2026-09-10"), dup_str("2026-09-09"),
                                malloc(sizeof(int)), 1, PRESTAMO_FINALIZADO, 0, 150.0 * 9 };
    memcpy(prestamos[2].ids_ejemplares, ids_p3, sizeof(ids_p3));

    int ids_p4[] = {1};
    prestamos[3] = (Prestamo){ 4, dup_str("333330000"), dup_str("2026-09-05"), dup_str("2026-09-12"), NULL,
                                malloc(sizeof(int)), 1, PRESTAMO_ACTIVO, 0, 0.0 };
    memcpy(prestamos[3].ids_ejemplares, ids_p4, sizeof(ids_p4));

    printf("========== BUSQUEDA SIMPLE (\"regimen totalitario\") ==========\n");
    busqueda_simple(catalogo, cantidad_libros, ejemplares, cantidad_ejemplares, "regimen totalitario");

    printf("\n========== BUSQUEDA SIMPLE (\"realismo\") ==========\n");
    busqueda_simple(catalogo, cantidad_libros, ejemplares, cantidad_ejemplares, "cien anios");

    printf("\n========== BUSQUEDA AVANZADA (genero=\"Distopia\" EXACTA, operador O) ==========\n");
    busqueda_avanzada(catalogo, cantidad_libros, NULL, MODO_CONTIENE, NULL, MODO_CONTIENE, "Distopia", MODO_EXACTA, NULL, MODO_CONTIENE, OPERADOR_O);

    printf("\n========== BUSQUEDA AVANZADA (genero CONTIENE \"magico\" Y autor CONTIENE \"cortazar\") ==========\n");
    busqueda_avanzada(catalogo, cantidad_libros, NULL, MODO_CONTIENE, "cortazar", MODO_CONTIENE, "magico", MODO_CONTIENE, NULL, MODO_CONTIENE, OPERADOR_Y);

    printf("\n========== ESTADISTICA A: top producciones ==========\n");
    estadistica_top_producciones(prestamos, cantidad_prestamos, ejemplares, cantidad_ejemplares);

    printf("\n========== ESTADISTICA B: top usuarios ==========\n");
    estadistica_top_usuarios(prestamos, cantidad_prestamos);

    printf("\n========== ESTADISTICA C: top mes-anio recaudado ==========\n");
    estadistica_top_meses_recaudado(prestamos, cantidad_prestamos);

    printf("\n========== ESTADISTICA D: generos extremos ==========\n");
    estadistica_generos_extremos(prestamos, cantidad_prestamos, ejemplares, cantidad_ejemplares, catalogo, cantidad_libros);

    /* limpieza de la memoria de prueba */
    for (int i = 0; i < cantidad_libros; i++) {
        free(catalogo[i].nombre); free(catalogo[i].autor);
        free(catalogo[i].genero); free(catalogo[i].resumen);
    }
    for (int i = 0; i < cantidad_ejemplares; i++) {
        free(ejemplares[i].nombre_libro);
    }
    for (int i = 0; i < cantidad_prestamos; i++) {
        free(prestamos[i].usuario_id);
        free(prestamos[i].fecha_inicio);
        free(prestamos[i].fecha_entrega_esperada);
        free(prestamos[i].fecha_devolucion);
        free(prestamos[i].ids_ejemplares);
    }

    return 0;
}
