#ifndef USUARIO_H
#define USUARIO_H

/* unicidad del usuario la da "identificacion" */
typedef struct {
    char *identificacion;
    char *nombre;
    char *direccion;
} Usuario;

/* lee todos los usuarios desde ARCHIVO_USUARIOS */
Usuario *leer_usuarios(int *cantidad);

/* persiste el arreglo completo de usuarios (sobrescribe) */
void guardar_usuarios(const Usuario *usuarios, int cantidad);

/* busca un usuario por identificacion. Devuelve el indice o -1 si no existe */
int buscar_usuario(const Usuario *usuarios, int cantidad, const char *identificacion);

/*
 * agrega un usuario nuevo al arreglo (por referencia, ya que crece)
 * Entradas: usuarios y cantidad por referencia, datos del nuevo usuario
 * Salidas: 1 si se creo, 0 si ya existia esa identificacion
 * Objetivo: cumplir la unicidad por identificacion al crear
 */
int crear_usuario(Usuario **usuarios, int *cantidad,
                   const char *identificacion, const char *nombre, const char *direccion);

/* modifica nombre/direccion de un usuario existente. Devuelve 1 si se aplico, 0 si no existe */
int modificar_usuario(Usuario *usuarios, int cantidad, const char *identificacion,
                       const char *nuevo_nombre, const char *nueva_direccion);

/*
 * elimina un usuario del arreglo (por referencia, ya que se achica)
 * nota: la validacion de "no tiene prestamos asociados" se hace en la capa de menu, consultando prestamo.h, para no acoplar este modulo a prestamos
 * devuelve 1 si se elimino, 0 si no existia
 */
int eliminar_usuario(Usuario **usuarios, int *cantidad, const char *identificacion);

/* imprime todos los usuarios */
void mostrar_usuarios(const Usuario *usuarios, int cantidad);

/* libera toda la memoria dinamica de usuarios (strings + arreglo) */
void liberar_usuarios(Usuario *usuarios, int cantidad);

#endif /* USUARIO_H */
