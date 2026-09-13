#ifndef BUSQUEDA_H
#define BUSQUEDA_H

#include "libro.h"
#include "ejemplar.h"

/* modo de comparacion de texto para un campo en la busqueda avanzada */
typedef enum {
    MODO_CONTIENE, MODO_EXACTA
} ModoTexto;

/* operador global entre los campos activos de la busqueda avanzada */
typedef enum {
    OPERADOR_O, OPERADOR_Y
} OperadorBusqueda;

/*
 * substring case insensitive 1 si buscado aparece dentro de texto
 * Entradas: texto sobre el que se busca, texto buscado
 * Salidas: 1 si hay coincidencia, 0 si no o si algun parametro es NULL/vacio
 * Objetivo: tecnica contiene reutilizable por busqueda simple y avanzada
 */
int texto_contiene(const char *texto, const char *buscado);

/*
 * comparacion exacta case insensitive de dos cadenas completas
 * Entradas: texto sobre el que se busca, texto buscado
 * Salidas: 1 si son iguales ignorando mayus/minus, 0 si no
 * Objetivo: tecnica exacta de la busqueda avanzada
 */
int texto_exacto(const char *texto, const char *buscado);

/*
 * indica si al menos un ejemplar de un libro esta disponible
 * Entradas: nombre del libro, arreglo de ejemplares y su cantidad
 * Salidas: 1 si hay al menos un ejemplar EJEMPLAR_DISPONIBLE para ese libro, 0 si no
 * Objetivo: calcular el "estado (disponible o no)" que pide la busqueda simple
 */
int libro_tiene_ejemplar_disponible(const char *nombre_libro, const Ejemplar *ejemplares, int cantidad_ejemplares);

/*
 * Busqueda simple: tecnica contiene + o (nombre, autor o resumen)
 * Entradas: catalogo y su cantidad, ejemplares y su cantidad (para el estado), texto a buscar
 * Salidas: ninguna, efecto imprime por cada match nombre, resumen y estado
 * Restricciones: no modifica catalogo ni ejemplares
 * Objetivo: cumplir Busqueda simple de la seccion 1.2 del enunciado
 */
void busqueda_simple(const Libro *catalogo, int cantidad_libros, const Ejemplar *ejemplares, int cantidad_ejemplares, const char *texto);

/*
 * Busqueda avanzada por campos opcionales (nombre, autor, genero, resumen)
 * Entradas: catalogo y su cantidad; por cada campo, el texto a buscar (NULL o "" significa no buscar ese campo) y su ModoTexto operador global entre los campos activos (o / y)
 * Salidas: ninguna (efecto: imprime nombre y resumen por cada match)
 * Restricciones: no modifica el catalogo
 * Objetivo: cumplir "Busqueda avanzada" de la seccion 1.2 del enunciado
 */
void busqueda_avanzada(const Libro *catalogo, int cantidad_libros, const char *campo_nombre, ModoTexto modo_nombre,
                        const char *campo_autor, ModoTexto modo_autor,
                        const char *campo_genero, ModoTexto modo_genero,
                        const char *campo_resumen, ModoTexto modo_resumen,
                        OperadorBusqueda operador);

#endif /* BUSQUEDA_H */
