#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/ejemplar.h"
#include "../include/constantes.h"
#include "../include/persistencia.h"
#include "cjson/cJSON.h"

/* convierte el enum de estado a texto para guardarlo en json */
static const char *estado_a_texto(EstadoEjemplar estado) {
    return (estado == EJEMPLAR_PRESTADO) ? "prestado" : "disponible";
}

/* convierte el texto leido del json de vuelta al enum */
static EstadoEjemplar texto_a_estado(const char *texto) {
    if (texto != NULL && strcmp(texto, "prestado") == 0) {
        return EJEMPLAR_PRESTADO;
    }
    return EJEMPLAR_DISPONIBLE;
}

Ejemplar *leer_ejemplares(int *cantidad) {
    *cantidad = 0;

    char *contenido = leer_archivo_completo(ARCHIVO_EJEMPLARES);
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

    Ejemplar *ejemplares = malloc(sizeof(Ejemplar) * (size_t)total);
    if (ejemplares == NULL) {
        cJSON_Delete(raiz);
        return NULL;
    }

    int agregados = 0;
    for (int i = 0; i < total; i++) {
        cJSON *item = cJSON_GetArrayItem(raiz, i);
        cJSON *jid = cJSON_GetObjectItemCaseSensitive(item, "id");
        cJSON *jnombre = cJSON_GetObjectItemCaseSensitive(item, "nombre_libro");
        cJSON *jestado = cJSON_GetObjectItemCaseSensitive(item, "estado");

        if (!cJSON_IsNumber(jid) || !cJSON_IsString(jnombre)) {
            continue; /* registro corrupto, se omite */
        }

        ejemplares[agregados].id = jid->valueint;
        ejemplares[agregados].nombre_libro = strdup(jnombre->valuestring);
        ejemplares[agregados].estado = texto_a_estado(
            cJSON_IsString(jestado) ? jestado->valuestring : NULL);
        agregados++;
    }

    cJSON_Delete(raiz);
    *cantidad = agregados;
    return ejemplares;
}

void guardar_ejemplares(const Ejemplar *ejemplares, int cantidad) {
    cJSON *raiz = cJSON_CreateArray();

    for (int i = 0; i < cantidad; i++) {
        cJSON *item = cJSON_CreateObject();
        cJSON_AddNumberToObject(item, "id", ejemplares[i].id);
        cJSON_AddStringToObject(item, "nombre_libro", ejemplares[i].nombre_libro);
        cJSON_AddStringToObject(item, "estado", estado_a_texto(ejemplares[i].estado));
        cJSON_AddItemToArray(raiz, item);
    }

    char *texto = cJSON_Print(raiz);
    if (texto != NULL) {
        escribir_archivo(ARCHIVO_EJEMPLARES, texto);
        free(texto);
    }
    cJSON_Delete(raiz);
}

int siguiente_id_ejemplar(const Ejemplar *ejemplares, int cantidad) {
    int mayor = 0;
    for (int i = 0; i < cantidad; i++) {
        if (ejemplares[i].id > mayor) {
            mayor = ejemplares[i].id;
        }
    }
    return mayor + 1;
}

Ejemplar *generar_ejemplares(const char *nombre_libro, int cantidad_nueva, const Ejemplar *existentes, int cantidad_existentes) {
    if (cantidad_nueva <= 0) {
        return NULL;
    }

    Ejemplar *nuevos = malloc(sizeof(Ejemplar) * (size_t)cantidad_nueva);
    if (nuevos == NULL) {
        return NULL;
    }

    int siguiente_id = siguiente_id_ejemplar(existentes, cantidad_existentes);

    for (int i = 0; i < cantidad_nueva; i++) {
        nuevos[i].id = siguiente_id + i;
        nuevos[i].nombre_libro = strdup(nombre_libro);
        nuevos[i].estado = EJEMPLAR_DISPONIBLE;
    }

    return nuevos;
}

int buscar_ejemplar_por_id(const Ejemplar *ejemplares, int cantidad, int id) {
    for (int i = 0; i < cantidad; i++) {
        if (ejemplares[i].id == id) {
            return i;
        }
    }
    return -1;
}

void liberar_ejemplares(Ejemplar *ejemplares, int cantidad) {
    for (int i = 0; i < cantidad; i++) {
        free(ejemplares[i].nombre_libro);
    }
    free(ejemplares);
}
