#ifndef FECHA_H
#define FECHA_H

/*
 * convierte una fecha en formato "YYYY-MM-DD" a un numero de dia serial
 * (dias desde 1970-01-01), lo que permite restar fechas para obtener
 * diferencias en dias y compararlas con <, >, ==
 * Entradas: texto de fecha "YYYY-MM-DD"
 * Salidas: numero de dia serial, o -1 si el texto no es una fecha valida
 * Objetivo: centralizar el manejo de fechas para todos los modulos que lo necesiten
 */
long fecha_a_dias(const char *texto);

/*
 * obtiene la fecha actual del sistema en formato "YYYY-MM-DD"
 * Entradas: buffer de al menos 11 bytes donde se escribe el resultado
 * Objetivo: tener una unica fuente de la fecha de sistema en todo el programa
 */
void fecha_sistema_actual(char *buffer, int tamano_buffer);

#endif /* FECHA_H */
