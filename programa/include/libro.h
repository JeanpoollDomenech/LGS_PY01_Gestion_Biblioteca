#ifndef LIBRO_H
#define LIBRO_H

/*
 * representa una obra del catalogo (no un ejemplar fisico individual
 * eso lo maneja ejemplar.h) La unicidad del catalogo la da "nombre"
 */
typedef struct {
    char *nombre;
    char *autor;
    int   anio_publicacion;
    char *genero;
    char *resumen;
    int   cantidad;   /* cantidad de ejemplares registrados para este libro */
} Libro;

/*
 * lee el catalogo completo desde ARCHIVO_LIBROS
 * Entradas: ninguna
 * Salidas: arreglo dinamico de Libro (o NULL si el archivo no existe/vacio)
 * Restricciones: el arreglo devuelto debe liberarse con liberar_catalogo()
 * Objetivo: cargar el estado persistido del catalogo a memoria
 */
Libro *leer_catalogo(int *cantidad);

/*
 * persiste el catalogo completo en ARCHIVO_LIBROS (sobrescribe)
 * Entradas: catalogo y su cantidad de elementos
 * Salidas: ninguna
 * Objetivo: guardar el estado actual del catalogo en disco
 */
void guardar_catalogo(const Libro *catalogo, int cantidad);

/*
 * busca un libro por nombre (unicidad del catalogo)
 * Entradas: catalogo, cantidad, nombre a buscar
 * Salidas: indice en el arreglo si existe, -1 si no
 * Objetivo: validar unicidad antes de insertar / ubicar un libro para lectura
 */
int buscar_libro_por_nombre(const Libro *catalogo, int cantidad, const char *nombre);

/*
 * parsea el archivo de carga en lote (formato delimitado por '#')
 * Entradas: ruta del archivo de lote
 * Salidas: arreglo dinamico de Libro candidatos a insertar; cantidad_leidos por referencia con cuantos se lograron parsear correctamente
 * Restricciones: no inserta en el catalogo real, solo parsea
 * Objetivo: separar el parseo del archivo de la logica de insercion
 */
Libro *cargar_lote_desde_archivo(const char *ruta, int *cantidad_leidos);

/*
 * inserta en el catalogo los libros del lote que sean nuevos (por nombre)
 * y con informacion valida, genera los ejemplares correspondientes, y muestra el reporte de los registros no procesados
 * Entradas: catalogo y cantidad por referencia (se modifican in-place), arreglo de lote y su cantidad
 * Salidas: ninguna (efecto: catalogo actualizado, reporte impreso)
 * Objetivo: cumplir la funcionalidad "incluir en lote"
 */
void procesar_lote(Libro **catalogo, int *cantidad_catalogo, const Libro *lote, int cantidad_lote);

/*
 * imprime el catalogo completo con toda su informacion
 */
void mostrar_catalogo(const Libro *catalogo, int cantidad);

/*
 * libera toda la memoria dinamica del catalogo (strings + arreglo)
 */
void liberar_catalogo(Libro *catalogo, int cantidad);

#endif /* LIBRO_H */
