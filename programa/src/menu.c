#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/menu.h"
#include "../include/constantes.h"
#include "../include/libro.h"
#include "../include/ejemplar.h"
#include "../include/usuario.h"
#include "../include/prestamo.h"
#include "../include/fecha.h"
#include "../include/busqueda.h"
#include "../include/estadisticas.h"

/*
 * tamano de buffer para leer una fecha escrita por el usuario con fgets
 * Debe ser mayor que LONGITUD_FECHA si el buffer fuera del tamano exacto de "YYYY-MM-DD", fgets llenaria el buffer sin dejar espacio para el \n final,
 * que quedaria pendiente en stdin y desalinearia la siguiente lectura
 */
#define TAM_ENTRADA_FECHA 32

/* ------------------------------------------------------------------ */
/* utilidades de lectura de entrada                                    */
/* ------------------------------------------------------------------ */

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

/* lee una linea de texto de stdin en buffer, quitando el salto de linea final */
static void leer_linea(char *buffer, int tamano) {
    if (fgets(buffer, tamano, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }
    size_t longitud = strlen(buffer);
    if (longitud > 0 && buffer[longitud - 1] == '\n') {
        buffer[longitud - 1] = '\0';
    }
}

/* ------------------------------------------------------------------ */
/* Gestion de catalogo                                                  */
/* ------------------------------------------------------------------ */

static void accion_incluir_lote(void) {
    char ruta[256];
    printf("Ruta del archivo de lote: ");
    leer_linea(ruta, sizeof(ruta));

    int cantidad_lote = 0;
    Libro *lote = cargar_lote_desde_archivo(ruta, &cantidad_lote);
    if (lote == NULL || cantidad_lote == 0) {
        printf("No se pudo leer el archivo indicado (o esta vacio).\n");
        if (lote != NULL) {
            liberar_catalogo(lote, cantidad_lote);
        }
        return;
    }

    int cantidad_catalogo = 0;
    Libro *catalogo = leer_catalogo(&cantidad_catalogo);
    int cantidad_ejemplares = 0;
    Ejemplar *ejemplares = leer_ejemplares(&cantidad_ejemplares);

    procesar_lote(&catalogo, &cantidad_catalogo, &ejemplares, &cantidad_ejemplares, lote, cantidad_lote);

    guardar_catalogo(catalogo, cantidad_catalogo);
    guardar_ejemplares(ejemplares, cantidad_ejemplares);

    liberar_catalogo(catalogo, cantidad_catalogo);
    liberar_ejemplares(ejemplares, cantidad_ejemplares);
    liberar_catalogo(lote, cantidad_lote);
}

static void accion_ver_catalogo(void) {
    int cantidad = 0;
    Libro *catalogo = leer_catalogo(&cantidad);
    printf("\n--- Catalogo completo ---\n");
    mostrar_catalogo(catalogo, cantidad);
    liberar_catalogo(catalogo, cantidad);
}

static void menu_gestion_catalogo(void) {
    int opcion;
    do {
        printf("\n--- Gestion de catalogo ---\n");
        printf("1. Incluir en lote\n");
        printf("2. Ver catalogo completo\n");
        printf("3. Volver\n");
        printf("Seleccione una opcion: ");

        opcion = leer_opcion();
        switch (opcion) {
            case 1: accion_incluir_lote(); break;
            case 2: accion_ver_catalogo(); break;
            case 3: break;
            default: printf("Opcion invalida, intente de nuevo.\n");
        }
    } while (opcion != 3);
}

/* ------------------------------------------------------------------ */
/* Gestion de usuarios                                                  */
/* ------------------------------------------------------------------ */

static void accion_crear_usuario(void) {
    char identificacion[64];
    char nombre[128];
    char direccion[256];

    printf("Identificacion: ");
    leer_linea(identificacion, sizeof(identificacion));
    printf("Nombre: ");
    leer_linea(nombre, sizeof(nombre));
    printf("Direccion: ");
    leer_linea(direccion, sizeof(direccion));

    int cantidad = 0;
    Usuario *usuarios = leer_usuarios(&cantidad);

    if (crear_usuario(&usuarios, &cantidad, identificacion, nombre, direccion)) {
        guardar_usuarios(usuarios, cantidad);
        printf("Usuario creado correctamente.\n");
    } else {
        printf("Ya existe un usuario con esa identificacion.\n");
    }

    liberar_usuarios(usuarios, cantidad);
}

static void accion_ver_usuarios(void) {
    int cantidad = 0;
    Usuario *usuarios = leer_usuarios(&cantidad);
    printf("\n--- Usuarios registrados ---\n");
    mostrar_usuarios(usuarios, cantidad);
    liberar_usuarios(usuarios, cantidad);
}

static void accion_modificar_usuario(void) {
    char identificacion[64];
    printf("Identificacion del usuario a modificar: ");
    leer_linea(identificacion, sizeof(identificacion));

    int cantidad = 0;
    Usuario *usuarios = leer_usuarios(&cantidad);

    if (buscar_usuario(usuarios, cantidad, identificacion) == -1) {
        printf("No existe un usuario con esa identificacion.\n");
        liberar_usuarios(usuarios, cantidad);
        return;
    }

    char nuevo_nombre[128];
    char nueva_direccion[256];
    printf("Nuevo nombre (enter para no modificar): ");
    leer_linea(nuevo_nombre, sizeof(nuevo_nombre));
    printf("Nueva direccion (enter para no modificar): ");
    leer_linea(nueva_direccion, sizeof(nueva_direccion));

    modificar_usuario(usuarios, cantidad, identificacion, nuevo_nombre, nueva_direccion);
    guardar_usuarios(usuarios, cantidad);
    printf("Usuario modificado correctamente.\n");

    liberar_usuarios(usuarios, cantidad);
}

static void accion_eliminar_usuario(void) {
    char identificacion[64];
    printf("Identificacion del usuario a eliminar: ");
    leer_linea(identificacion, sizeof(identificacion));

    int cantidad_usuarios = 0;
    Usuario *usuarios = leer_usuarios(&cantidad_usuarios);
    int cantidad_prestamos = 0;
    Prestamo *prestamos = leer_prestamos(&cantidad_prestamos);

    if (buscar_usuario(usuarios, cantidad_usuarios, identificacion) == -1) {
        printf("No existe un usuario con esa identificacion.\n");
    } else if (usuario_tiene_prestamos(identificacion, prestamos, cantidad_prestamos)) {
        printf("No se puede eliminar: el usuario tiene prestamos asociados.\n");
    } else {
        eliminar_usuario(&usuarios, &cantidad_usuarios, identificacion);
        guardar_usuarios(usuarios, cantidad_usuarios);
        printf("Usuario eliminado correctamente.\n");
    }

    liberar_usuarios(usuarios, cantidad_usuarios);
    liberar_prestamos(prestamos, cantidad_prestamos);
}

static void menu_gestion_usuarios(void) {
    int opcion;
    do {
        printf("\n--- Gestion de usuarios ---\n");
        printf("1. Crear usuario\n");
        printf("2. Ver usuarios\n");
        printf("3. Modificar usuario\n");
        printf("4. Eliminar usuario\n");
        printf("5. Volver\n");
        printf("Seleccione una opcion: ");

        opcion = leer_opcion();
        switch (opcion) {
            case 1: accion_crear_usuario(); break;
            case 2: accion_ver_usuarios(); break;
            case 3: accion_modificar_usuario(); break;
            case 4: accion_eliminar_usuario(); break;
            case 5: break;
            default: printf("Opcion invalida, intente de nuevo.\n");
        }
    } while (opcion != 5);
}

/* ------------------------------------------------------------------ */
/* Historial y vencimiento de prestamos                                 */
/* ------------------------------------------------------------------ */

static void accion_historial(void) {
    char desde[TAM_ENTRADA_FECHA];
    char hasta[TAM_ENTRADA_FECHA];

    printf("Fecha inicio del rango (YYYY-MM-DD): ");
    leer_linea(desde, sizeof(desde));
    printf("Fecha fin del rango (YYYY-MM-DD): ");
    leer_linea(hasta, sizeof(hasta));

    int cantidad_prestamos = 0;
    Prestamo *prestamos = leer_prestamos(&cantidad_prestamos);
    int cantidad_ejemplares = 0;
    Ejemplar *ejemplares = leer_ejemplares(&cantidad_ejemplares);

    printf("\n--- Historial de prestamos (%s a %s) ---\n", desde, hasta);
    mostrar_historial(prestamos, cantidad_prestamos, desde, hasta, ejemplares, cantidad_ejemplares);

    liberar_prestamos(prestamos, cantidad_prestamos);
    liberar_ejemplares(ejemplares, cantidad_ejemplares);
}

static void accion_vencimientos(void) {
    char fecha_hoy[LONGITUD_FECHA];
    fecha_sistema_actual(fecha_hoy, sizeof(fecha_hoy));

    int cantidad_prestamos = 0;
    Prestamo *prestamos = leer_prestamos(&cantidad_prestamos);
    int cantidad_ejemplares = 0;
    Ejemplar *ejemplares = leer_ejemplares(&cantidad_ejemplares);

    printf("\n--- Vencimiento de prestamos (fecha de sistema: %s) ---\n", fecha_hoy);
    mostrar_vencimientos(prestamos, cantidad_prestamos, fecha_hoy, ejemplares, cantidad_ejemplares);

    liberar_prestamos(prestamos, cantidad_prestamos);
    liberar_ejemplares(ejemplares, cantidad_ejemplares);
}

/*
 * pide un modo de comparacion de texto (contiene/exacta) para un campo
 * de la busqueda avanzada; se usa solo si el campo no viene vacio
 */
static ModoTexto pedir_modo_texto(const char *nombre_campo) {
    printf("  Modo para %s (1=contiene, 2=exacta): ", nombre_campo);
    int opcion = leer_opcion();
    return (opcion == 2) ? MODO_EXACTA : MODO_CONTIENE;
}

static void accion_estadisticas(void) {
    int cantidad_prestamos = 0;
    Prestamo *prestamos = leer_prestamos(&cantidad_prestamos);
    int cantidad_ejemplares = 0;
    Ejemplar *ejemplares = leer_ejemplares(&cantidad_ejemplares);
    int cantidad_catalogo = 0;
    Libro *catalogo = leer_catalogo(&cantidad_catalogo);

    printf("\n--- Estadisticas ---\n");
    estadistica_top_producciones(prestamos, cantidad_prestamos, ejemplares, cantidad_ejemplares);
    printf("\n");
    estadistica_top_usuarios(prestamos, cantidad_prestamos);
    printf("\n");
    estadistica_top_meses_recaudado(prestamos, cantidad_prestamos);
    printf("\n");
    estadistica_generos_extremos(prestamos, cantidad_prestamos, ejemplares, cantidad_ejemplares,
                                  catalogo, cantidad_catalogo);

    liberar_prestamos(prestamos, cantidad_prestamos);
    liberar_ejemplares(ejemplares, cantidad_ejemplares);
    liberar_catalogo(catalogo, cantidad_catalogo);
}

static void menu_estadisticas(void) {
    accion_estadisticas();
}

static void menu_operativo_historial(void) {
    accion_historial();
}

static void menu_operativo_vencimientos(void) {
    accion_vencimientos();
}

/* ------------------------------------------------------------------ */
/* Prestamo y devolucion de ejemplares (Opciones Generales)             */
/* ------------------------------------------------------------------ */

static void accion_prestamo(void) {
    char usuario_id[64];
    printf("Identificacion del usuario: ");
    leer_linea(usuario_id, sizeof(usuario_id));

    int cantidad_usuarios = 0;
    Usuario *usuarios = leer_usuarios(&cantidad_usuarios);
    int existe_usuario = (buscar_usuario(usuarios, cantidad_usuarios, usuario_id) != -1);
    liberar_usuarios(usuarios, cantidad_usuarios);

    if (!existe_usuario) {
        printf("No existe un usuario con esa identificacion.\n");
        return;
    }

    char fecha_inicio[TAM_ENTRADA_FECHA];
    char fecha_entrega[TAM_ENTRADA_FECHA];
    printf("Fecha de inicio (YYYY-MM-DD): ");
    leer_linea(fecha_inicio, sizeof(fecha_inicio));
    printf("Fecha de entrega esperada (YYYY-MM-DD): ");
    leer_linea(fecha_entrega, sizeof(fecha_entrega));

    printf("Cuantos ejemplares desea prestar? ");
    int cantidad_solicitados = leer_opcion();
    if (cantidad_solicitados <= 0) {
        printf("Cantidad invalida.\n");
        return;
    }

    int *ids_solicitados = malloc(sizeof(int) * (size_t)cantidad_solicitados);
    for (int i = 0; i < cantidad_solicitados; i++) {
        printf("Id del ejemplar #%d: ", i + 1);
        ids_solicitados[i] = leer_opcion();
    }

    int cantidad_ejemplares = 0;
    Ejemplar *ejemplares = leer_ejemplares(&cantidad_ejemplares);

    int *ids_disponibles = malloc(sizeof(int) * (size_t)cantidad_solicitados);
    int cantidad_disponibles = 0;

    printf("\n--- Verificacion de disponibilidad ---\n");
    for (int i = 0; i < cantidad_solicitados; i++) {
        int id = ids_solicitados[i];
        int indice = buscar_ejemplar_por_id(ejemplares, cantidad_ejemplares, id);

        if (indice == -1) {
            printf("  [NO DISPONIBLE] ejemplar %d no existe\n", id);
            continue;
        }
        if (ejemplares[indice].estado != EJEMPLAR_DISPONIBLE) {
            printf("  [NO DISPONIBLE] ejemplar %d ya esta prestado\n", id);
            continue;
        }

        printf("  [OK] ejemplar %d (%s) disponible\n", id, ejemplares[indice].nombre_libro);
        ids_disponibles[cantidad_disponibles] = id;
        cantidad_disponibles++;
    }

    if (cantidad_disponibles == 0) {
        printf("Ningun ejemplar solicitado esta disponible. No se genero el prestamo.\n");
        free(ids_solicitados);
        free(ids_disponibles);
        liberar_ejemplares(ejemplares, cantidad_ejemplares);
        return;
    }

    /* marcar como prestados y persistir el nuevo estado de stock */
    for (int i = 0; i < cantidad_disponibles; i++) {
        int indice = buscar_ejemplar_por_id(ejemplares, cantidad_ejemplares, ids_disponibles[i]);
        ejemplares[indice].estado = EJEMPLAR_PRESTADO;
    }
    guardar_ejemplares(ejemplares, cantidad_ejemplares);

    int cantidad_prestamos = 0;
    Prestamo *prestamos = leer_prestamos(&cantidad_prestamos);
    Prestamo comprobante = crear_prestamo(&prestamos, &cantidad_prestamos, usuario_id, fecha_inicio, fecha_entrega, ids_disponibles, cantidad_disponibles);
    guardar_prestamos(prestamos, cantidad_prestamos);

    printf("\n--- Comprobante de prestamo ---\n");
    printf("Id de prestamo: %d\n", comprobante.id);
    printf("Usuario: %s\n", usuario_id);
    printf("Fecha inicio: %s\n", fecha_inicio);
    printf("Fecha de entrega: %s\n", fecha_entrega);
    printf("Ejemplares prestados: ");
    for (int i = 0; i < cantidad_disponibles; i++) {
        int indice = buscar_ejemplar_por_id(ejemplares, cantidad_ejemplares, ids_disponibles[i]);
        printf("%d (%s)%s", ids_disponibles[i], ejemplares[indice].nombre_libro, (i < cantidad_disponibles - 1) ? ", " : "\n");
    }

    liberar_prestamos(prestamos, cantidad_prestamos);
    liberar_ejemplares(ejemplares, cantidad_ejemplares);
    free(ids_solicitados);
    free(ids_disponibles);
}

static void accion_devolucion(void) {
    printf("Id del prestamo a devolver: ");
    int id_prestamo = leer_opcion();

    char fecha_devolucion[TAM_ENTRADA_FECHA];
    printf("Fecha de devolucion (YYYY-MM-DD): ");
    leer_linea(fecha_devolucion, sizeof(fecha_devolucion));

    int cantidad_prestamos = 0;
    Prestamo *prestamos = leer_prestamos(&cantidad_prestamos);

    int indice = -1;
    for (int i = 0; i < cantidad_prestamos; i++) {
        if (prestamos[i].id == id_prestamo) {
            indice = i;
            break;
        }
    }

    if (indice == -1) {
        printf("No existe un prestamo con ese id.\n");
        liberar_prestamos(prestamos, cantidad_prestamos);
        return;
    }
    if (prestamos[indice].fecha_devolucion != NULL) {
        printf("Ese prestamo ya fue devuelto anteriormente.\n");
        liberar_prestamos(prestamos, cantidad_prestamos);
        return;
    }

    int entrega_tardia = 0;
    double monto = calcular_monto_devolucion(prestamos[indice].fecha_inicio, prestamos[indice].fecha_entrega_esperada, fecha_devolucion, &entrega_tardia);

    prestamos[indice].fecha_devolucion = strdup(fecha_devolucion);
    prestamos[indice].estado = PRESTAMO_FINALIZADO;
    prestamos[indice].entrega_tardia = entrega_tardia;
    prestamos[indice].monto = monto;

    guardar_prestamos(prestamos, cantidad_prestamos);

    /* liberar el stock: los ejemplares vuelven a estar disponibles */
    int cantidad_ejemplares = 0;
    Ejemplar *ejemplares = leer_ejemplares(&cantidad_ejemplares);
    for (int j = 0; j < prestamos[indice].cantidad_ejemplares; j++) {
        int idx = buscar_ejemplar_por_id(ejemplares, cantidad_ejemplares, prestamos[indice].ids_ejemplares[j]);
        if (idx != -1) {
            ejemplares[idx].estado = EJEMPLAR_DISPONIBLE;
        }
    }
    guardar_ejemplares(ejemplares, cantidad_ejemplares);

    printf("\n--- Comprobante de devolucion ---\n");
    printf("Id de prestamo: %d\n", prestamos[indice].id);
    printf("Entrega tardia: %s\n", entrega_tardia ? "si" : "no");
    printf("Monto a cancelar: %.2f\n", monto);

    liberar_prestamos(prestamos, cantidad_prestamos);
    liberar_ejemplares(ejemplares, cantidad_ejemplares);
}

static void accion_busqueda_simple(void) {
    char texto[256];
    printf("Texto a buscar (en nombre, autor o resumen): ");
    leer_linea(texto, sizeof(texto));

    int cantidad_catalogo = 0;
    Libro *catalogo = leer_catalogo(&cantidad_catalogo);
    int cantidad_ejemplares = 0;
    Ejemplar *ejemplares = leer_ejemplares(&cantidad_ejemplares);

    printf("\n--- Resultados de busqueda simple ---\n");
    busqueda_simple(catalogo, cantidad_catalogo, ejemplares, cantidad_ejemplares, texto);

    liberar_catalogo(catalogo, cantidad_catalogo);
    liberar_ejemplares(ejemplares, cantidad_ejemplares);
}

static void accion_busqueda_avanzada(void) {
    char campo_nombre[256], campo_autor[256], campo_genero[256], campo_resumen[256];
    ModoTexto modo_nombre = MODO_CONTIENE, modo_autor = MODO_CONTIENE;
    ModoTexto modo_genero = MODO_CONTIENE, modo_resumen = MODO_CONTIENE;

    printf("Buscar por nombre (enter para omitir): ");
    leer_linea(campo_nombre, sizeof(campo_nombre));
    if (campo_nombre[0] != '\0') {
        modo_nombre = pedir_modo_texto("nombre");
    }

    printf("Buscar por autor (enter para omitir): ");
    leer_linea(campo_autor, sizeof(campo_autor));
    if (campo_autor[0] != '\0') {
        modo_autor = pedir_modo_texto("autor");
    }

    printf("Buscar por genero (enter para omitir): ");
    leer_linea(campo_genero, sizeof(campo_genero));
    if (campo_genero[0] != '\0') {
        modo_genero = pedir_modo_texto("genero");
    }

    printf("Buscar por resumen (enter para omitir): ");
    leer_linea(campo_resumen, sizeof(campo_resumen));
    if (campo_resumen[0] != '\0') {
        modo_resumen = pedir_modo_texto("resumen");
    }

    printf("Operador entre campos (1=o, 2=y): ");
    int opcion_operador = leer_opcion();
    OperadorBusqueda operador = (opcion_operador == 2) ? OPERADOR_Y : OPERADOR_O;

    int cantidad_catalogo = 0;
    Libro *catalogo = leer_catalogo(&cantidad_catalogo);

    printf("\n--- Resultados de busqueda avanzada ---\n");
    busqueda_avanzada(catalogo, cantidad_catalogo,
                       campo_nombre, modo_nombre,
                       campo_autor, modo_autor,
                       campo_genero, modo_genero,
                       campo_resumen, modo_resumen,
                       operador);

    liberar_catalogo(catalogo, cantidad_catalogo);
}

/* ------------------------------------------------------------------ */
/* Menus principales                                                    */
/* ------------------------------------------------------------------ */

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
            case 1: menu_gestion_catalogo(); break;
            case 2: menu_gestion_usuarios(); break;
            case 3: menu_operativo_historial(); break;
            case 4: menu_operativo_vencimientos(); break;
            case 5: menu_estadisticas(); break;
            case 6: break;
            default: printf("Opcion invalida, intente de nuevo.\n");
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
            case 1: accion_busqueda_simple(); break;
            case 2: accion_busqueda_avanzada(); break;
            case 3: accion_prestamo(); break;
            case 4: accion_devolucion(); break;
            case 5: break;
            default: printf("Opcion invalida, intente de nuevo.\n");
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
            case 1: ejecutar_menu_operativo(); break;
            case 2: ejecutar_menu_general(); break;
            case 3: printf("Hasta pronto.\n"); break;
            default: printf("Opcion invalida, intente de nuevo.\n");
        }
    } while (opcion != 3);
}
