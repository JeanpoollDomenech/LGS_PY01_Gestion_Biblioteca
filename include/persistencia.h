#ifndef PERSISTENCIA_H
#define PERSISTENCIA_H

/*
 * lee un archivo completo a memoria como un string
 * Entradas: ruta del archivo
 * Salidas: buffer dinamico con el contenido (o NULL si no existe/error)
 * Restricciones: el buffer devuelto debe liberarse con free()
 * Objetivo: servir de base para pasarle contenido crudo a cJSON_Parse()
 */
char *leer_archivo_completo(const char *ruta);

/*
 * escribe (sobrescribe) un archivo con el contenido dado
 * Entradas: ruta del archivo, contenido a escribir
 * Salidas: 1 si tuvo exito, 0 si hubo error
 * Objetivo: servir de base para volcar el resultado de cJSON_Print() a disco
 */
int escribir_archivo(const char *ruta, const char *contenido);

#endif /* PERSISTENCIA_H */
