#include <stdio.h>
#include "../include/menu.h"

/*
 * lee una linea de stdin y la interpreta como entero
 * devuelve -1 si la entrada no es un numero valido (para no trabar el menu con scanf mal alineado)
 */
static int leer_opcion(void) {
    char buffer[32];
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        return -1;
    }

    int opcion;
    if (sscanf(buffer, "%d", &opcion) != 1) {
        return -1;
    }
    return opcion;
}

void ejecutar_menu_operativo(void) {
    int opcion;
    do {
        printf("\n--- Opciones Operativas ---\n");
        printf("1. Gestion de catalogo\n");
        printf("2. Gestion de usuarios\n");
        printf("3. Historial de prestamos\n");
        printf("4. Vencimiento de prestamos\n");
        printf("5. Estadisticas\n");
        printf("6. Volver\n");
        printf("Seleccione una opcion: ");

        opcion = leer_opcion();

        switch (opcion) {
            case 1:
                printf("Funcion pendiente: gestion de catalogo.\n");
                break;
            case 2:
                printf("Funcion pendiente: gestion de usuarios.\n");
                break;
            case 3:
                printf("Funcion pendiente: historial de prestamos.\n");
                break;
            case 4:
                printf("Funcion pendiente: vencimiento de prestamos.\n");
                break;
            case 5:
                printf("Funcion pendiente: estadisticas.\n");
                break;
            case 6:
                break;
            default:
                printf("Opcion invalida, intente de nuevo.\n");
        }
    } while (opcion != 6);
}

void ejecutar_menu_general(void) {
    int opcion;
    do {
        printf("\n--- Opciones Generales ---\n");
        printf("1. Busqueda simple\n");
        printf("2. Busqueda avanzada\n");
        printf("3. Prestamo de ejemplares\n");
        printf("4. Devolucion de ejemplares\n");
        printf("5. Volver\n");
        printf("Seleccione una opcion: ");

        opcion = leer_opcion();

        switch (opcion) {
            case 1:
                printf("Funcion pendiente: busqueda simple.\n");
                break;
            case 2:
                printf("Funcion pendiente: busqueda avanzada.\n");
                break;
            case 3:
                printf("Funcion pendiente: prestamo de ejemplares.\n");
                break;
            case 4:
                printf("Funcion pendiente: devolucion de ejemplares.\n");
                break;
            case 5:
                break;
            default:
                printf("Opcion invalida, intente de nuevo.\n");
        }
    } while (opcion != 5);
}

void ejecutar_menu_principal(void) {
    int opcion;
    do {
        printf("\n=== Sistema de Gestion de Biblioteca ===\n");
        printf("1. Opciones Operativas\n");
        printf("2. Opciones Generales\n");
        printf("3. Salir\n");
        printf("Seleccione una opcion: ");

        opcion = leer_opcion();

        switch (opcion) {
            case 1:
                ejecutar_menu_operativo();
                break;
            case 2:
                ejecutar_menu_general();
                break;
            case 3:
                printf("Hasta pronto.\n");
                break;
            default:
                printf("Opcion invalida, intente de nuevo.\n");
        }
    } while (opcion != 3);
}
