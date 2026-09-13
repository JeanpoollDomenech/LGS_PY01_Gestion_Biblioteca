#ifndef ESTADISTICAS_H
#define ESTADISTICAS_H

#include "prestamo.h"
#include "ejemplar.h"
#include "libro.h"

/*
 * A. Top 3 producciones (titulos del catalogo) mas prestadas, con cantidad
 * Entradas: prestamos y su cantidad, ejemplares y su cantidad para resolver a que libro pertenece cada ejemplar prestado
 * Salidas: ninguna efecto imprime el top 3 con su cantidad de prestamos
 * Objetivo: reporte A de Estadisticas, seccion 1.1 del enunciado
 */
void estadistica_top_producciones(const Prestamo *prestamos, int cantidad_prestamos, const Ejemplar *ejemplares, int cantidad_ejemplares);

/*
 * B. Top 3 usuarios con mas prestamos, con cantidad
 * Objetivo: reporte B de Estadisticas
 */
void estadistica_top_usuarios(const Prestamo *prestamos, int cantidad_prestamos);

/*
 * C. Top 5 mes-anio segun fecha_inicio con mayor monto recaudado
 * Restricciones: solo considera prestamos con monto > 0 ya finalizados/cobrados
 * Objetivo: reporte C de Estadisticas
 */
void estadistica_top_meses_recaudado(const Prestamo *prestamos, int cantidad_prestamos);

/*
 * D. Genero con mas y con menos prestamos solo generos con prestamos generados
 * Entradas: prestamos, ejemplares y catalogo (con sus cantidades) para resolver ejemplar -> libro -> genero
 * Objetivo: reporte D de Estadisticas
 */
void estadistica_generos_extremos(const Prestamo *prestamos, int cantidad_prestamos, const Ejemplar *ejemplares, int cantidad_ejemplares,
                                   const Libro *catalogo, int cantidad_libros);

#endif /* ESTADISTICAS_H */
