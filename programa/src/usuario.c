#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/usuario.h"
#include "../include/constantes.h"
#include "../include/persistencia.h"
#include "cjson/cJSON.h"

Usuario *leer_usuarios(int *cantidad) {
    *cantidad = 0;

    char *contenido = leer_archivo_completo(ARCHIVO_USUARIOS);
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

    Usuario *usuarios = malloc(sizeof(Usuario) * (size_t)total);
    if (usuarios == NULL) {
        cJSON_Delete(raiz);
        return NULL;
    }

    int agregados = 0;
    for (int i = 0; i < total; i++) {
        cJSON *item = cJSON_GetArrayItem(raiz, i);
        cJSON *jid = cJSON_GetObjectItemCaseSensitive(item, "identificacion");
        cJSON *jnombre = cJSON_GetObjectItemCaseSensitive(item, "nombre");
        cJSON *jdireccion = cJSON_GetObjectItemCaseSensitive(item, "direccion");

        if (!cJSON_IsString(jid) || !cJSON_IsString(jnombre) || !cJSON_IsString(jdireccion)) {
            continue; /* registro corrupto, se omite */
        }

        usuarios[agregados].identificacion = strdup(jid->valuestring);
        usuarios[agregados].nombre = strdup(jnombre->valuestring);
        usuarios[agregados].direccion = strdup(jdireccion->valuestring);
        agregados++;
    }

    cJSON_Delete(raiz);
    *cantidad = agregados;
    return usuarios;
}

void guardar_usuarios(const Usuario *usuarios, int cantidad) {
    cJSON *raiz = cJSON_CreateArray();

    for (int i = 0; i < cantidad; i++) {
        cJSON *item = cJSON_CreateObject();
        cJSON_AddStringToObject(item, "identificacion", usuarios[i].identificacion);
        cJSON_AddStringToObject(item, "nombre", usuarios[i].nombre);
        cJSON_AddStringToObject(item, "direccion", usuarios[i].direccion);
        cJSON_AddItemToArray(raiz, item);
    }

    char *texto = cJSON_Print(raiz);
    if (texto != NULL) {
        escribir_archivo(ARCHIVO_USUARIOS, texto);
        free(texto);
    }
    cJSON_Delete(raiz);
}

int buscar_usuario(const Usuario *usuarios, int cantidad, const char *identificacion) {
    for (int i = 0; i < cantidad; i++) {
        if (strcmp(usuarios[i].identificacion, identificacion) == 0) {
            return i;
        }
    }
    return -1;
}

int crear_usuario(Usuario **usuarios, int *cantidad,
                   const char *identificacion, const char *nombre, const char *direccion) {
    if (buscar_usuario(*usuarios, *cantidad, identificacion) != -1) {
        return 0; /* ya existe: se viola la unicidad por identificacion */
    }

    Usuario *ampliado = realloc(*usuarios, sizeof(Usuario) * (size_t)(*cantidad + 1));
    if (ampliado == NULL) {
        return 0;
    }
    *usuarios = ampliado;

    (*usuarios)[*cantidad].identificacion = strdup(identificacion);
    (*usuarios)[*cantidad].nombre = strdup(nombre);
    (*usuarios)[*cantidad].direccion = strdup(direccion);
    (*cantidad)++;

    return 1;
}

int modificar_usuario(Usuario *usuarios, int cantidad, const char *identificacion,
                       const char *nuevo_nombre, const char *nueva_direccion) {
    int indice = buscar_usuario(usuarios, cantidad, identificacion);
    if (indice == -1) {
        return 0;
    }

    if (nuevo_nombre != NULL && strlen(nuevo_nombre) > 0) {
        free(usuarios[indice].nombre);
        usuarios[indice].nombre = strdup(nuevo_nombre);
    }
    if (nueva_direccion != NULL && strlen(nueva_direccion) > 0) {
        free(usuarios[indice].direccion);
        usuarios[indice].direccion = strdup(nueva_direccion);
    }

    return 1;
}

int eliminar_usuario(Usuario **usuarios, int *cantidad, const char *identificacion) {
    int indice = buscar_usuario(*usuarios, *cantidad, identificacion);
    if (indice == -1) {
        return 0;
    }

    free((*usuarios)[indice].identificacion);
    free((*usuarios)[indice].nombre);
    free((*usuarios)[indice].direccion);

    for (int i = indice; i < *cantidad - 1; i++) {
        (*usuarios)[i] = (*usuarios)[i + 1];
    }
    (*cantidad)--;

    if (*cantidad > 0) {
        Usuario *reducido = realloc(*usuarios, sizeof(Usuario) * (size_t)(*cantidad));
        if (reducido != NULL) {
            *usuarios = reducido;
        }
    } else {
        free(*usuarios);
        *usuarios = NULL;
    }

    return 1;
}

void mostrar_usuarios(const Usuario *usuarios, int cantidad) {
    if (cantidad == 0) {
        printf("No hay usuarios registrados.\n");
        return;
    }
    for (int i = 0; i < cantidad; i++) {
        printf("- %s | %s | %s\n",
               usuarios[i].identificacion, usuarios[i].nombre, usuarios[i].direccion);
    }
}

void liberar_usuarios(Usuario *usuarios, int cantidad) {
    for (int i = 0; i < cantidad; i++) {
        free(usuarios[i].identificacion);
        free(usuarios[i].nombre);
        free(usuarios[i].direccion);
    }
    free(usuarios);
}
