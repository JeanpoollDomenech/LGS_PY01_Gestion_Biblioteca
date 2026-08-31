#include <stdio.h>
#include <stdlib.h>
#include "../include/persistencia.h"

char *leer_archivo_completo(const char *ruta) {
    FILE *archivo = fopen(ruta, "rb");
    if (archivo == NULL) {
        return NULL;
    }

    fseek(archivo, 0, SEEK_END);
    long tamano = ftell(archivo);
    fseek(archivo, 0, SEEK_SET);

    if (tamano < 0) {
        fclose(archivo);
        return NULL;
    }

    char *buffer = malloc((size_t)tamano + 1);
    if (buffer == NULL) {
        fclose(archivo);
        return NULL;
    }

    size_t leidos = fread(buffer, 1, (size_t)tamano, archivo);
    buffer[leidos] = '\0';

    fclose(archivo);
    return buffer;
}

int escribir_archivo(const char *ruta, const char *contenido) {
    FILE *archivo = fopen(ruta, "wb");
    if (archivo == NULL) {
        return 0;
    }

    size_t longitud = 0;
    while (contenido[longitud] != '\0') {
        longitud++;
    }

    size_t escritos = fwrite(contenido, 1, longitud, archivo);
    fclose(archivo);

    return escritos == longitud;
}
