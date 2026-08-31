#ifndef EJEMPLAR_H
#define EJEMPLAR_H


typedef enum {
    EJEMPLAR_DISPONIBLE,
    EJEMPLAR_PRESTADO
} EstadoEjemplar;

/*
 * representa un ejemplar fisico individual de un Libro del catalogo
 * se relaciona con Libro por "nombre_libro" (la unicidad del catalogo)
 */
typedef struct {
    int id;
    char *nombre_libro;
    EstadoEjemplar estado;
} Ejemplar;

/* lee todos los ejemplares desde ARCHIVO_EJEMPLARES */
Ejemplar *leer_ejemplares(int *cantidad);

/* persiste el arreglo completo de ejemplares (sobrescribe) */
void guardar_ejemplares(const Ejemplar *ejemplares, int cantidad);

/*
 * calcula el siguiente id disponible en base a los ejemplares existentes
 * Objetivo: generar identificadores unicos e incrementales
 */
int siguiente_id_ejemplar(const Ejemplar *ejemplares, int cantidad);

/*
 * genera "cantidad_nueva" ejemplares nuevos para un libro (todos EJEMPLAR_DISPONIBLE)
 * Entradas: nombre del libro, cantidad a generar, ejemplares existentes (para calcular ids sin colisiones)
 * Salidas: arreglo dinamico con los ejemplares nuevos generados
 * Objetivo: soportar la generacion de ejemplares al cargar/ampliar el catalogo
 */
Ejemplar *generar_ejemplares(const char *nombre_libro, int cantidad_nueva, const Ejemplar *existentes, int cantidad_existentes);

/* busca un ejemplar por id, devuelve el indice o -1 si no existe. */
int buscar_ejemplar_por_id(const Ejemplar *ejemplares, int cantidad, int id);

/* libera la memoria dinamica del arreglo de ejemplares. */
void liberar_ejemplares(Ejemplar *ejemplares, int cantidad);

#endif /* EJEMPLAR_H */
