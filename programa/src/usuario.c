#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/usuario.h"

Usuario *leer_usuarios(int *cantidad) {
    /* lunes 31 integrar cJSON para leer data/usuarios.json */
    *cantidad = 0;
    return NULL;
}

void guardar_usuarios(const Usuario *usuarios, int cantidad) {
    (void)usuarios;
    (void)cantidad;
    /* lunes 31 integrar cJSON para escribir data/usuarios.json */
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
    (void)usuarios;
    (void)cantidad;
    (void)identificacion;
    (void)nombre;
    (void)direccion;
    /* lunes 31 validar unicidad + realloc del arreglo + insertar */
    return 0;
}

int modificar_usuario(Usuario *usuarios, int cantidad, const char *identificacion,
                       const char *nuevo_nombre, const char *nueva_direccion) {
    (void)usuarios;
    (void)cantidad;
    (void)identificacion;
    (void)nuevo_nombre;
    (void)nueva_direccion;
    /* lunes 31 */
    return 0;
}

int eliminar_usuario(Usuario **usuarios, int *cantidad, const char *identificacion) {
    (void)usuarios;
    (void)cantidad;
    (void)identificacion;
    /* lunes 31 la validacion de "sin prestamos asociados" se hace en la capa de menu, llamando primero a usuario_tiene_prestamos() */
    return 0;
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
