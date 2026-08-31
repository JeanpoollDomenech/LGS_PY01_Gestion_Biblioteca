#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/prestamo.h"

Prestamo *leer_prestamos(int *cantidad) {
    /* jueves 3 integrar cJSON para leer data/prestamos.json */
    *cantidad = 0;
    return NULL;
}

void guardar_prestamos(const Prestamo *prestamos, int cantidad) {
    (void)prestamos;
    (void)cantidad;
    /* jueves 3 integrar cJSON para escribir data/prestamos.json */
}

int siguiente_id_prestamo(const Prestamo *prestamos, int cantidad) {
    int mayor = 0;
    for (int i = 0; i < cantidad; i++) {
        if (prestamos[i].id > mayor) {
            mayor = prestamos[i].id;
        }
    }
    return mayor + 1;
}

int usuario_tiene_prestamos(const char *identificacion_usuario,
                             const Prestamo *prestamos, int cantidad) {
    for (int i = 0; i < cantidad; i++) {
        if (strcmp(prestamos[i].usuario_id, identificacion_usuario) == 0) {
            return 1;
        }
    }
    return 0;
}

Prestamo crear_prestamo(Prestamo **prestamos, int *cantidad,
                         const char *usuario_id, const char *fecha_inicio,
                         const char *fecha_entrega_esperada,
                         const int *ids_ejemplares, int cantidad_ejemplares) {
    (void)prestamos;
    (void)cantidad;
    (void)usuario_id;
    (void)fecha_inicio;
    (void)fecha_entrega_esperada;
    (void)ids_ejemplares;
    (void)cantidad_ejemplares;
    /* jueves 3 verificar disponibilidad + generar comprobante + gestion de stock */
    Prestamo vacio;
    vacio.id = -1;
    vacio.usuario_id = NULL;
    vacio.fecha_inicio = NULL;
    vacio.fecha_entrega_esperada = NULL;
    vacio.fecha_devolucion = NULL;
    vacio.ids_ejemplares = NULL;
    vacio.cantidad_ejemplares = 0;
    vacio.estado = PRESTAMO_ACTIVO;
    vacio.entrega_tardia = 0;
    vacio.monto = 0.0;
    return vacio;
}

double calcular_monto_devolucion(const char *fecha_inicio,
                                  const char *fecha_entrega_esperada,
                                  const char *fecha_devolucion,
                                  int *entrega_tardia) {
    (void)fecha_inicio;
    (void)fecha_entrega_esperada;
    (void)fecha_devolucion;
    /* viernes 4 calcular dias, aplicar tabla de tarifas por tramos */
    *entrega_tardia = 0;
    return 0.0;
}

void mostrar_historial(const Prestamo *prestamos, int cantidad,
                        const char *fecha_desde, const char *fecha_hasta) {
    (void)prestamos;
    (void)cantidad;
    (void)fecha_desde;
    (void)fecha_hasta;
    /* sabado 5 - domingo 6 */
    printf("Funcion pendiente: historial de prestamos.\n");
}

void mostrar_vencimientos(const Prestamo *prestamos, int cantidad,
                           const char *fecha_sistema) {
    (void)prestamos;
    (void)cantidad;
    (void)fecha_sistema;
    /* sabado 5 - domingo 6 */
    printf("Funcion pendiente: vencimiento de prestamos.\n");
}

void liberar_prestamos(Prestamo *prestamos, int cantidad) {
    for (int i = 0; i < cantidad; i++) {
        free(prestamos[i].usuario_id);
        free(prestamos[i].fecha_inicio);
        free(prestamos[i].fecha_entrega_esperada);
        free(prestamos[i].fecha_devolucion);
        free(prestamos[i].ids_ejemplares);
    }
    free(prestamos);
}
