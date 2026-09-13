#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "../include/busqueda.h"

int texto_contiene(const char *texto, const char *buscado) {
    if (texto == NULL || buscado == NULL || buscado[0] == '\0') {
        return 0;
    }

    size_t len_texto = strlen(texto);
    size_t len_buscado = strlen(buscado);
    if (len_buscado > len_texto) {
        return 0;
    }

    for (size_t i = 0; i + len_buscado <= len_texto; i++) {
        size_t j = 0;
        while (j < len_buscado &&
               tolower((unsigned char)texto[i + j]) == tolower((unsigned char)buscado[j])) {
            j++;
        }
        if (j == len_buscado) {
            return 1;
        }
    }
    return 0;
}

int texto_exacto(const char *texto, const char *buscado) {
    if (texto == NULL || buscado == NULL) {
        return 0;
    }
    if (strlen(texto) != strlen(buscado)) {
        return 0;
    }
    for (size_t i = 0; texto[i] != '\0'; i++) {
        if (tolower((unsigned char)texto[i]) != tolower((unsigned char)buscado[i])) {
            return 0;
        }
    }
    return 1;
}

int libro_tiene_ejemplar_disponible(const char *nombre_libro,
                                     const Ejemplar *ejemplares, int cantidad_ejemplares) {
    for (int i = 0; i < cantidad_ejemplares; i++) {
        if (ejemplares[i].nombre_libro != NULL &&
            strcmp(ejemplares[i].nombre_libro, nombre_libro) == 0 &&
            ejemplares[i].estado == EJEMPLAR_DISPONIBLE) {
            return 1;
        }
    }
    return 0;
}

void busqueda_simple(const Libro *catalogo, int cantidad_libros,
                      const Ejemplar *ejemplares, int cantidad_ejemplares,
                      const char *texto) {
    int encontrados = 0;

    if (texto == NULL || texto[0] == '\0') {
        printf("Debe indicar un texto de busqueda.\n");
        return;
    }

    for (int i = 0; i < cantidad_libros; i++) {
        int coincide = texto_contiene(catalogo[i].nombre, texto) ||
                        texto_contiene(catalogo[i].autor, texto) ||
                        texto_contiene(catalogo[i].resumen, texto);

        if (coincide) {
            int disponible = libro_tiene_ejemplar_disponible(catalogo[i].nombre,
                                                               ejemplares, cantidad_ejemplares);
            printf("- %s | %s | %s\n",
                   catalogo[i].nombre,
                   catalogo[i].resumen,
                   disponible ? "disponible" : "no disponible");
            encontrados++;
        }
    }

    if (encontrados == 0) {
        printf("No se encontraron coincidencias para \"%s\".\n", texto);
    }
}

void busqueda_avanzada(const Libro *catalogo, int cantidad_libros,
                        const char *campo_nombre, ModoTexto modo_nombre,
                        const char *campo_autor, ModoTexto modo_autor,
                        const char *campo_genero, ModoTexto modo_genero,
                        const char *campo_resumen, ModoTexto modo_resumen,
                        OperadorBusqueda operador) {
    int encontrados = 0;

    int nombre_activo  = campo_nombre  != NULL && campo_nombre[0]  != '\0';
    int autor_activo   = campo_autor   != NULL && campo_autor[0]   != '\0';
    int genero_activo  = campo_genero  != NULL && campo_genero[0]  != '\0';
    int resumen_activo = campo_resumen != NULL && campo_resumen[0] != '\0';

    if (!nombre_activo && !autor_activo && !genero_activo && !resumen_activo) {
        printf("Debe indicar al menos un campo de busqueda.\n");
        return;
    }

    for (int i = 0; i < cantidad_libros; i++) {
        int coincide_nombre = nombre_activo &&
            (modo_nombre == MODO_EXACTA ? texto_exacto(catalogo[i].nombre, campo_nombre)
                                         : texto_contiene(catalogo[i].nombre, campo_nombre));
        int coincide_autor = autor_activo &&
            (modo_autor == MODO_EXACTA ? texto_exacto(catalogo[i].autor, campo_autor)
                                        : texto_contiene(catalogo[i].autor, campo_autor));
        int coincide_genero = genero_activo &&
            (modo_genero == MODO_EXACTA ? texto_exacto(catalogo[i].genero, campo_genero)
                                         : texto_contiene(catalogo[i].genero, campo_genero));
        int coincide_resumen = resumen_activo &&
            (modo_resumen == MODO_EXACTA ? texto_exacto(catalogo[i].resumen, campo_resumen)
                                          : texto_contiene(catalogo[i].resumen, campo_resumen));

        int coincide;
        if (operador == OPERADOR_Y) {
            coincide = (!nombre_activo  || coincide_nombre) &&
                       (!autor_activo   || coincide_autor)  &&
                       (!genero_activo  || coincide_genero) &&
                       (!resumen_activo || coincide_resumen);
        } else {
            coincide = (nombre_activo  && coincide_nombre) ||
                       (autor_activo   && coincide_autor)  ||
                       (genero_activo  && coincide_genero) ||
                       (resumen_activo && coincide_resumen);
        }

        if (coincide) {
            printf("- %s | %s\n", catalogo[i].nombre, catalogo[i].resumen);
            encontrados++;
        }
    }

    if (encontrados == 0) {
        printf("No se encontraron coincidencias.\n");
    }
}
