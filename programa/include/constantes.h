#ifndef CONSTANTES_H
#define CONSTANTES_H

/* rutas de los archivos JSON */
#define ARCHIVO_LIBROS      "data/libros.json"
#define ARCHIVO_EJEMPLARES  "data/ejemplares.json"
#define ARCHIVO_USUARIOS    "data/usuarios.json"
#define ARCHIVO_PRESTAMOS   "data/prestamos.json"

/* reglas de negocio */
#define DIAS_PROXIMO_VENCIMIENTO 5   /* ventana para "proximo a vencer" */

/* tramos de tarifas de prestamo en colones */
#define LIMITE_DIAS_TRAMO_1 7        /* 1 a 7 dias */
#define LIMITE_DIAS_TRAMO_2 15       /* 8 a 15 dias, 16+ es el ultimo tramo */

#define TARIFA_DIA_TRAMO_1    175
#define TARIFA_TARDIA_TRAMO_1 100
#define TARIFA_DIA_TRAMO_2    150
#define TARIFA_TARDIA_TRAMO_2 75
#define TARIFA_DIA_TRAMO_3    100
#define TARIFA_TARDIA_TRAMO_3 50

/* limites de formato no de cantidad de registros porque esos son dinamicos */
#define LONGITUD_FECHA 11            /* "YYYY-MM-DD" + "\0" */

#endif /* CONSTANTES_H */
