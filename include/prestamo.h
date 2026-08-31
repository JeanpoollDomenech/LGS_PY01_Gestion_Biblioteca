#ifndef PRESTAMO_H
#define PRESTAMO_H

typedef enum {
    PRESTAMO_ACTIVO,
    PRESTAMO_VENCIDO,
    PRESTAMO_FINALIZADO
} EstadoPrestamo;

typedef struct {
    int id;
    char *usuario_id;              /* identificacion del usuario */
    char *fecha_inicio;            /* "YYYY-MM-DD" */
    char *fecha_entrega_esperada;  /* "YYYY-MM-DD" */
    char *fecha_devolucion;        /* NULL si aun no se ha devuelto */
    int  *ids_ejemplares;
    int   cantidad_ejemplares;
    EstadoPrestamo estado;
    int   entrega_tardia;          /* solo tiene sentido si esta finalizado */
    double monto;                  /* calculado al devolver, 0 mientras activo */
} Prestamo;

/* lee todos los prestamos desde ARCHIVO_PRESTAMOS */
Prestamo *leer_prestamos(int *cantidad);

/* persiste el arreglo completo de prestamos (sobrescribe) */
void guardar_prestamos(const Prestamo *prestamos, int cantidad);

/* calcula el siguiente id disponible de prestamo */
int siguiente_id_prestamo(const Prestamo *prestamos, int cantidad);

/*
 * indica si un usuario tiene al menos un prestamo asociado (de cualquier estado)
 * usado por la capa de menu antes de permitir eliminar un usuario
 */
int usuario_tiene_prestamos(const char *identificacion_usuario,
                             const Prestamo *prestamos, int cantidad);

/*
 * crea un prestamo nuevo verificando disponibilidad de cada ejemplar en el
 * rango de fechas indicado, no hace la verificacion de disponibilidad en si (eso esta en ejemplar.h, la capa de menu que cruza ambos modulos
 * este metodo asume que ya se validaron los ids de ejemplares a prestar
 * devuelve el prestamo creado (o un Prestamo con id -1 si algo fallo)
 */
Prestamo crear_prestamo(Prestamo **prestamos, int *cantidad,
                         const char *usuario_id, const char *fecha_inicio,
                         const char *fecha_entrega_esperada,
                         const int *ids_ejemplares, int cantidad_ejemplares);

/*
 * calcula el monto a cobrar por una devolucion segun la tabla de tarifas
 * por tramos (ver constantes.h) y si hubo entrega tardia
 * Entradas: fecha de inicio, fecha de entrega esperada, fecha real de devolucion
 * Salidas: monto calculado; entrega_tardia por referencia (1/0)
 */
double calcular_monto_devolucion(const char *fecha_inicio,
                                  const char *fecha_entrega_esperada,
                                  const char *fecha_devolucion,
                                  int *entrega_tardia);

/* muestra el historial de prestamos en un rango de fechas (por fecha de entrega) */
void mostrar_historial(const Prestamo *prestamos, int cantidad,
                        const char *fecha_desde, const char *fecha_hasta);

/* muestra prestamos vencidos y proximos a vencer segun la fecha de sistema */
void mostrar_vencimientos(const Prestamo *prestamos, int cantidad,
                           const char *fecha_sistema);

/* libera toda la memoria dinamica de prestamos (strings, arreglos de ids, arreglo) */
void liberar_prestamos(Prestamo *prestamos, int cantidad);

#endif /* PRESTAMO_H */
