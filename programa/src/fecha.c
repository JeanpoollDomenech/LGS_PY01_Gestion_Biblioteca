#include <stdio.h>
#include <time.h>
#include "../include/fecha.h"

long fecha_a_dias(const char *texto) {
    int anio, mes, dia;
    if (texto == NULL || sscanf(texto, "%d-%d-%d", &anio, &mes, &dia) != 3) {
        return -1;
    }
    if (mes < 1 || mes > 12 || dia < 1 || dia > 31) {
        return -1;
    }

    /* algoritmo "days_from_civil" (Howard Hinnant), da dias desde 1970-01-01 */
    anio -= (mes <= 2) ? 1 : 0;
    long era = (anio >= 0 ? anio : anio - 399) / 400;
    long anio_de_era = anio - era * 400;
    long dia_del_anio = (153 * (mes + (mes > 2 ? -3 : 9)) + 2) / 5 + dia - 1;
    long dia_de_era = anio_de_era * 365 + anio_de_era / 4 - anio_de_era / 100 + dia_del_anio;

    return era * 146097 + dia_de_era - 719468;
}

void fecha_sistema_actual(char *buffer, int tamano_buffer) {
    time_t ahora = time(NULL);
    struct tm *tm_local = localtime(&ahora);
    strftime(buffer, (size_t)tamano_buffer, "%Y-%m-%d", tm_local);
}
