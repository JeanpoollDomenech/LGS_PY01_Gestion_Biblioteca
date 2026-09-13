#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/libro.h"
#include "../include/constantes.h"
#include "../include/persistencia.h"
#include "cjson/cJSON.h"

#define CAMPOS_LOTE 6

Libro *leer_catalogo(int *cantidad) {
    *cantidad = 0;

    char *contenido = leer_archivo_completo(ARCHIVO_LIBROS);
    if (contenido == NULL) {
        return NULL;
    }

    cJSON *raiz = cJSON_Parse(contenido);
    free(contenido);
    if (raiz == NULL || !cJSON_IsArray(raiz)) {
        cJSON_Delete(raiz);
        return NULL;
    }

    int total = cJSON_GetArraySize(raiz);
    if (total == 0) {
        cJSON_Delete(raiz);
        return NULL;
    }

    Libro *catalogo = malloc(sizeof(Libro) * (size_t)total);
    if (catalogo == NULL) {
        cJSON_Delete(raiz);
        return NULL;
    }

    int agregados = 0;
    for (int i = 0; i < total; i++) {
        cJSON *item = cJSON_GetArrayItem(raiz, i);
        cJSON *jnombre = cJSON_GetObjectItemCaseSensitive(item, "nombre");
        cJSON *jautor = cJSON_GetObjectItemCaseSensitive(item, "autor");
        cJSON *janio = cJSON_GetObjectItemCaseSensitive(item, "anio_publicacion");
        cJSON *jgenero = cJSON_GetObjectItemCaseSensitive(item, "genero");
        cJSON *jresumen = cJSON_GetObjectItemCaseSensitive(item, "resumen");
        cJSON *jcantidad = cJSON_GetObjectItemCaseSensitive(item, "cantidad");

        if (!cJSON_IsString(jnombre) || !cJSON_IsString(jautor) || !cJSON_IsNumber(janio) ||
            !cJSON_IsString(jgenero) || !cJSON_IsString(jresumen) || !cJSON_IsNumber(jcantidad)) {
            continue; /* registro corrupto, se omite */
        }

        catalogo[agregados].nombre = strdup(jnombre->valuestring);
        catalogo[agregados].autor = strdup(jautor->valuestring);
        catalogo[agregados].anio_publicacion = janio->valueint;
        catalogo[agregados].genero = strdup(jgenero->valuestring);
        catalogo[agregados].resumen = strdup(jresumen->valuestring);
        catalogo[agregados].cantidad = jcantidad->valueint;
        agregados++;
    }

    cJSON_Delete(raiz);
    *cantidad = agregados;
    return catalogo;
}

void guardar_catalogo(const Libro *catalogo, int cantidad) {
    cJSON *raiz = cJSON_CreateArray();

    for (int i = 0; i < cantidad; i++) {
        cJSON *item = cJSON_CreateObject();
        cJSON_AddStringToObject(item, "nombre", catalogo[i].nombre);
        cJSON_AddStringToObject(item, "autor", catalogo[i].autor);
        cJSON_AddNumberToObject(item, "anio_publicacion", catalogo[i].anio_publicacion);
        cJSON_AddStringToObject(item, "genero", catalogo[i].genero);
        cJSON_AddStringToObject(item, "resumen", catalogo[i].resumen);
        cJSON_AddNumberToObject(item, "cantidad", catalogo[i].cantidad);
        cJSON_AddItemToArray(raiz, item);
    }

    char *texto = cJSON_Print(raiz);
    if (texto != NULL) {
        escribir_archivo(ARCHIVO_LIBROS, texto);
        free(texto);
    }
    cJSON_Delete(raiz);
}

int buscar_libro_por_nombre(const Libro *catalogo, int cantidad, const char *nombre) {
    for (int i = 0; i < cantidad; i++) {
        if (strcmp(catalogo[i].nombre, nombre) == 0) {
            return i;
        }
    }
    return -1;
}

/* quita espacios/tabs al inicio y espacios/tabs/\r/\n al final, inplace */
static char *recortar(char *cadena) {
    while (*cadena == ' ' || *cadena == '\t') {
        cadena++;
    }
    if (*cadena == '\0') {
        return cadena;
    }
    char *fin = cadena + strlen(cadena) - 1;
    while (fin > cadena && (*fin == ' ' || *fin == '\t' || *fin == '\r' || *fin == '\n')) {
        *fin = '\0';
        fin--;
    }
    return cadena;
}

/*
 * divide linea en hasta max_campos usando # como delimitador, modificando
 * la linea inplace (reemplaza # por \0). Devuelve cuantos campos se lograron ubicar
 */
static int dividir_linea(char *linea, char *campos[], int max_campos) {
    int n = 0;
    char *actual = linea;
    while (n < max_campos) {
        campos[n] = actual;
        n++;
        char *siguiente = strchr(actual, '#');
        if (siguiente == NULL) {
            break;
        }
        *siguiente = '\0';
        actual = siguiente + 1;
    }
    return n;
}

/* intenta convertir un texto a entero de forma estricta -1 si no es valido */
static int texto_a_entero(const char *texto) {
    if (texto == NULL || *texto == '\0') {
        return -1;
    }
    char *fin;
    long valor = strtol(texto, &fin, 10);
    if (*fin != '\0') {
        return -1;
    }
    return (int)valor;
}

/*
 * lee una linea completa de un archivo hacia un buffer dinamico, haciendo crecer el buffer con realloc si la linea es mas larga que la capacidad actual
 * Reemplaza a getline() esa funcion es una extension gnu/posix que no existe en el runtime de windows o mingww64, por lo que usarla directamente rompe la
 * compilacion en windows aunque funcione perfecto en linux Con esta version
 * Entradas: puntero al buffer (puede reemplazarse internamente via realloc), puntero a su capacidad actual, archivo abierto en modo lectura
 * Salidas: cantidad de caracteres leidos (incluye el \n si estaba presente), o -1 si se llego a EOF sin leer ningun caracter
 * Restricciones: el buffer debe liberarse con free() por quien llama a esta funcion
 * Objetivo: lectura de lineas de largo arbitrario, portable, para cargar_lote_desde_archivo
 */
static long leer_linea_archivo(char **buffer, size_t *capacidad, FILE *archivo) {
    if (*buffer == NULL || *capacidad == 0) {
        *capacidad = 256;
        *buffer = malloc(*capacidad);
        if (*buffer == NULL) {
            return -1;
        }
    }

    size_t longitud = 0;
    (*buffer)[0] = '\0';

    while (1) {
        if (fgets(*buffer + longitud, (int)(*capacidad - longitud), archivo) == NULL) {
            return (longitud > 0) ? (long)longitud : -1;
        }

        longitud += strlen(*buffer + longitud);

        if (longitud > 0 && (*buffer)[longitud - 1] == '\n') {
            return (long)longitud;
        }

        if (longitud + 1 >= *capacidad) {
            size_t nueva_capacidad = (*capacidad) * 2;
            char *ampliado = realloc(*buffer, nueva_capacidad);
            if (ampliado == NULL) {
                return (long)longitud; /* memoria insuficiente: se devuelve lo leido hasta ahora */
            }
            *buffer = ampliado;
            *capacidad = nueva_capacidad;
        } else {
            /* fgets no lleno el buffer y no hallo \n se llego a EOF sin salto de linea final */
            return (long)longitud;
        }
    }
}

Libro *cargar_lote_desde_archivo(const char *ruta, int *cantidad_leidos) {
    *cantidad_leidos = 0;

    FILE *archivo = fopen(ruta, "r");
    if (archivo == NULL) {
        return NULL;
    }

    Libro *lote = NULL;
    int capacidad = 0;
    int cantidad = 0;

    char *linea = NULL;
    size_t tam_buffer = 0;
    long leidos;

    while ((leidos = leer_linea_archivo(&linea, &tam_buffer, archivo)) != -1) {
        if (leidos <= 0) {
            continue;
        }

        /* saltar lineas en blanco */
        int solo_blancos = 1;
        for (char *p = linea; *p != '\0'; p++) {
            if (*p != ' ' && *p != '\t' && *p != '\r' && *p != '\n') {
                solo_blancos = 0;
                break;
            }
        }
        if (solo_blancos) {
            continue;
        }

        char *campos[CAMPOS_LOTE];
        int encontrados = dividir_linea(linea, campos, CAMPOS_LOTE);

        if (cantidad == capacidad) {
            capacidad = (capacidad == 0) ? 8 : capacidad * 2;
            Libro *tmp = realloc(lote, sizeof(Libro) * (size_t)capacidad);
            if (tmp == NULL) {
                break;
            }
            lote = tmp;
        }

        Libro *actual = &lote[cantidad];

        actual->nombre = strdup(encontrados >= 1 ? recortar(campos[0]) : "");
        actual->autor = strdup(encontrados >= 2 ? recortar(campos[1]) : "");
        actual->anio_publicacion = (encontrados >= 3) ? texto_a_entero(recortar(campos[2])) : -1;
        actual->genero = strdup(encontrados >= 4 ? recortar(campos[3]) : "");
        actual->resumen = strdup(encontrados >= 5 ? recortar(campos[4]) : "");
        actual->cantidad = (encontrados >= 6) ? texto_a_entero(recortar(campos[5])) : -1;

        cantidad++;
    }

    free(linea);
    fclose(archivo);

    *cantidad_leidos = cantidad;
    return lote;
}

void procesar_lote(Libro **catalogo, int *cantidad_catalogo,
                    Ejemplar **ejemplares, int *cantidad_ejemplares,
                    const Libro *lote, int cantidad_lote) {
    int insertados = 0;

    printf("\n--- Reporte de carga en lote (%d registro(s) leidos del archivo) ---\n", cantidad_lote);

    for (int i = 0; i < cantidad_lote; i++) {
        const Libro *candidato = &lote[i];
        const char *motivo = NULL;

        if (candidato->nombre == NULL || strlen(candidato->nombre) == 0) {
            motivo = "nombre vacio o linea con formato invalido";
        } else if (candidato->autor == NULL || strlen(candidato->autor) == 0) {
            motivo = "autor vacio";
        } else if (candidato->anio_publicacion <= 0) {
            motivo = "anio de publicacion invalido";
        } else if (candidato->genero == NULL || strlen(candidato->genero) == 0) {
            motivo = "genero vacio";
        } else if (candidato->resumen == NULL || strlen(candidato->resumen) == 0) {
            motivo = "resumen vacio";
        } else if (candidato->cantidad <= 0) {
            motivo = "cantidad invalida";
        } else if (buscar_libro_por_nombre(*catalogo, *cantidad_catalogo, candidato->nombre) != -1) {
            motivo = "ya existe en el catalogo (nombre duplicado)";
        }

        if (motivo != NULL) {
            printf("  [NO PROCESADO] \"%s\": %s\n",
                   (candidato->nombre != NULL && strlen(candidato->nombre) > 0) ? candidato->nombre : "(sin nombre)",
                   motivo);
            continue;
        }

        Libro *catalogo_ampliado = realloc(*catalogo, sizeof(Libro) * (size_t)(*cantidad_catalogo + 1));
        if (catalogo_ampliado == NULL) {
            printf("  [ERROR] memoria insuficiente para insertar \"%s\"\n", candidato->nombre);
            continue;
        }
        *catalogo = catalogo_ampliado;

        Libro *destino = &(*catalogo)[*cantidad_catalogo];
        destino->nombre = strdup(candidato->nombre);
        destino->autor = strdup(candidato->autor);
        destino->anio_publicacion = candidato->anio_publicacion;
        destino->genero = strdup(candidato->genero);
        destino->resumen = strdup(candidato->resumen);
        destino->cantidad = candidato->cantidad;
        (*cantidad_catalogo)++;

        Ejemplar *nuevos = generar_ejemplares(destino->nombre, destino->cantidad,
                                               *ejemplares, *cantidad_ejemplares);
        if (nuevos != NULL) {
            Ejemplar *ejemplares_ampliados = realloc(
                *ejemplares, sizeof(Ejemplar) * (size_t)(*cantidad_ejemplares + destino->cantidad));
            if (ejemplares_ampliados != NULL) {
                *ejemplares = ejemplares_ampliados;
                for (int j = 0; j < destino->cantidad; j++) {
                    (*ejemplares)[*cantidad_ejemplares + j] = nuevos[j];
                }
                *cantidad_ejemplares += destino->cantidad;
            }
            free(nuevos); /* solo se libera el arreglo temporal, los strings ya se movieron */
        }

        printf("  [OK] \"%s\" agregado con %d ejemplar(es)\n", destino->nombre, destino->cantidad);
        insertados++;
    }

    printf("--- Fin del reporte: %d insertado(s) / %d no procesado(s) ---\n",
           insertados, cantidad_lote - insertados);
}

void mostrar_catalogo(const Libro *catalogo, int cantidad) {
    if (cantidad == 0) {
        printf("El catalogo esta vacio.\n");
        return;
    }
    for (int i = 0; i < cantidad; i++) {
        printf("- %s (%s, %d) | genero: %s | cantidad: %d\n  resumen: %s\n",
               catalogo[i].nombre, catalogo[i].autor,
               catalogo[i].anio_publicacion, catalogo[i].genero,
               catalogo[i].cantidad, catalogo[i].resumen);
    }
}

void liberar_catalogo(Libro *catalogo, int cantidad) {
    for (int i = 0; i < cantidad; i++) {
        free(catalogo[i].nombre);
        free(catalogo[i].autor);
        free(catalogo[i].genero);
        free(catalogo[i].resumen);
    }
    free(catalogo);
}
