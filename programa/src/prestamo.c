#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/prestamo.h"
#include "../include/constantes.h"
#include "../include/persistencia.h"
#include "../include/fecha.h"
#include "cjson/cJSON.h"

static const char *estado_a_texto(EstadoPrestamo estado) {
    switch (estado) {
        case PRESTAMO_VENCIDO:    return "vencido";
        case PRESTAMO_FINALIZADO: return "finalizado";
        default:                  return "activo";
    }
}

static EstadoPrestamo texto_a_estado(const char *texto) {
    if (texto != NULL && strcmp(texto, "vencido") == 0) {
        return PRESTAMO_VENCIDO;
    }
    if (texto != NULL && strcmp(texto, "finalizado") == 0) {
        return PRESTAMO_FINALIZADO;
    }
    return PRESTAMO_ACTIVO;
}

/* busca el nombre del libro de un ejemplar dado su id desconocido si no se encuentra */
static const char *nombre_de_ejemplar(int id, const Ejemplar *ejemplares, int cantidad_ejemplares) {
    int indice = buscar_ejemplar_por_id(ejemplares, cantidad_ejemplares, id);
    return (indice != -1) ? ejemplares[indice].nombre_libro : "(desconocido)";
}

/* imprime id1 (nombre1), id2 (nombre2)..." para los ejemplares de un prestamo */
static void imprimir_ejemplares_prestamo(const Prestamo *prestamo,
                                          const Ejemplar *ejemplares, int cantidad_ejemplares) {
    for (int i = 0; i < prestamo->cantidad_ejemplares; i++) {
        int id = prestamo->ids_ejemplares[i];
        printf("%d (%s)%s", id, nombre_de_ejemplar(id, ejemplares, cantidad_ejemplares),
               (i < prestamo->cantidad_ejemplares - 1) ? ", " : "");
    }
}

Prestamo *leer_prestamos(int *cantidad) {
    *cantidad = 0;

    char *contenido = leer_archivo_completo(ARCHIVO_PRESTAMOS);
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

    Prestamo *prestamos = malloc(sizeof(Prestamo) * (size_t)total);
    if (prestamos == NULL) {
        cJSON_Delete(raiz);
        return NULL;
    }

    int agregados = 0;
    for (int i = 0; i < total; i++) {
        cJSON *item = cJSON_GetArrayItem(raiz, i);
        cJSON *jid = cJSON_GetObjectItemCaseSensitive(item, "id");
        cJSON *jusuario = cJSON_GetObjectItemCaseSensitive(item, "usuario_id");
        cJSON *jinicio = cJSON_GetObjectItemCaseSensitive(item, "fecha_inicio");
        cJSON *jentrega = cJSON_GetObjectItemCaseSensitive(item, "fecha_entrega_esperada");
        cJSON *jdevolucion = cJSON_GetObjectItemCaseSensitive(item, "fecha_devolucion");
        cJSON *jids = cJSON_GetObjectItemCaseSensitive(item, "ids_ejemplares");
        cJSON *jestado = cJSON_GetObjectItemCaseSensitive(item, "estado");
        cJSON *jtardia = cJSON_GetObjectItemCaseSensitive(item, "entrega_tardia");
        cJSON *jmonto = cJSON_GetObjectItemCaseSensitive(item, "monto");

        if (!cJSON_IsNumber(jid) || !cJSON_IsString(jusuario) || !cJSON_IsString(jinicio) ||
            !cJSON_IsString(jentrega) || !cJSON_IsArray(jids)) {
            continue; /* registro corrupto, se omite */
        }

        Prestamo *destino = &prestamos[agregados];
        destino->id = jid->valueint;
        destino->usuario_id = strdup(jusuario->valuestring);
        destino->fecha_inicio = strdup(jinicio->valuestring);
        destino->fecha_entrega_esperada = strdup(jentrega->valuestring);
        destino->fecha_devolucion = cJSON_IsString(jdevolucion) ? strdup(jdevolucion->valuestring) : NULL;

        int cant_ids = cJSON_GetArraySize(jids);
        destino->ids_ejemplares = (cant_ids > 0) ? malloc(sizeof(int) * (size_t)cant_ids) : NULL;
        for (int j = 0; j < cant_ids; j++) {
            cJSON *jid_item = cJSON_GetArrayItem(jids, j);
            destino->ids_ejemplares[j] = cJSON_IsNumber(jid_item) ? jid_item->valueint : 0;
        }
        destino->cantidad_ejemplares = cant_ids;

        destino->estado = texto_a_estado(cJSON_IsString(jestado) ? jestado->valuestring : NULL);
        destino->entrega_tardia = cJSON_IsNumber(jtardia) ? jtardia->valueint : 0;
        destino->monto = cJSON_IsNumber(jmonto) ? jmonto->valuedouble : 0.0;

        agregados++;
    }

    cJSON_Delete(raiz);
    *cantidad = agregados;
    return prestamos;
}

void guardar_prestamos(const Prestamo *prestamos, int cantidad) {
    cJSON *raiz = cJSON_CreateArray();

    for (int i = 0; i < cantidad; i++) {
        cJSON *item = cJSON_CreateObject();
        cJSON_AddNumberToObject(item, "id", prestamos[i].id);
        cJSON_AddStringToObject(item, "usuario_id", prestamos[i].usuario_id);
        cJSON_AddStringToObject(item, "fecha_inicio", prestamos[i].fecha_inicio);
        cJSON_AddStringToObject(item, "fecha_entrega_esperada", prestamos[i].fecha_entrega_esperada);
        if (prestamos[i].fecha_devolucion != NULL) {
            cJSON_AddStringToObject(item, "fecha_devolucion", prestamos[i].fecha_devolucion);
        } else {
            cJSON_AddNullToObject(item, "fecha_devolucion");
        }

        cJSON *arreglo_ids = cJSON_CreateArray();
        for (int j = 0; j < prestamos[i].cantidad_ejemplares; j++) {
            cJSON_AddItemToArray(arreglo_ids, cJSON_CreateNumber(prestamos[i].ids_ejemplares[j]));
        }
        cJSON_AddItemToObject(item, "ids_ejemplares", arreglo_ids);

        cJSON_AddStringToObject(item, "estado", estado_a_texto(prestamos[i].estado));
        cJSON_AddNumberToObject(item, "entrega_tardia", prestamos[i].entrega_tardia);
        cJSON_AddNumberToObject(item, "monto", prestamos[i].monto);

        cJSON_AddItemToArray(raiz, item);
    }

    char *texto = cJSON_Print(raiz);
    if (texto != NULL) {
        escribir_archivo(ARCHIVO_PRESTAMOS, texto);
        free(texto);
    }
    cJSON_Delete(raiz);
}

int siguiente_id_prestamo(const Prestamo *prestamos, int cantidad) {
    int mayor = 0;
    for (int i = 0; i < cantidad; i++) {
        if (prestamos[i].id > mayor) {
            mayor = prestamos[i].id;
        }
    }
    return mayor + 1;
}

int usuario_tiene_prestamos(const char *identificacion_usuario,
                             const Prestamo *prestamos, int cantidad) {
    for (int i = 0; i < cantidad; i++) {
        if (strcmp(prestamos[i].usuario_id, identificacion_usuario) == 0) {
            return 1;
        }
    }
    return 0;
}

Prestamo crear_prestamo(Prestamo **prestamos, int *cantidad, const char *usuario_id, const char *fecha_inicio, const char *fecha_entrega_esperada, const int *ids_ejemplares, int cantidad_ejemplares) {
    Prestamo nuevo;
    nuevo.id = siguiente_id_prestamo(*prestamos, *cantidad);
    nuevo.usuario_id = strdup(usuario_id);
    nuevo.fecha_inicio = strdup(fecha_inicio);
    nuevo.fecha_entrega_esperada = strdup(fecha_entrega_esperada);
    nuevo.fecha_devolucion = NULL;
    nuevo.cantidad_ejemplares = cantidad_ejemplares;
    nuevo.ids_ejemplares = (cantidad_ejemplares > 0) ? malloc(sizeof(int) * (size_t)cantidad_ejemplares) : NULL;
    for (int i = 0; i < cantidad_ejemplares; i++) {
        nuevo.ids_ejemplares[i] = ids_ejemplares[i];
    }
    nuevo.estado = PRESTAMO_ACTIVO;
    nuevo.entrega_tardia = 0;
    nuevo.monto = 0.0;

    Prestamo *ampliado = realloc(*prestamos, sizeof(Prestamo) * (size_t)(*cantidad + 1));
    if (ampliado == NULL) {
        /* no se pudo registrar: se libera lo reservado y se devuelve un prestamo invalido (id -1) */
        free(nuevo.usuario_id);
        free(nuevo.fecha_inicio);
        free(nuevo.fecha_entrega_esperada);
        free(nuevo.ids_ejemplares);
        nuevo.id = -1;
        return nuevo;
    }
    *prestamos = ampliado;
    (*prestamos)[*cantidad] = nuevo;
    (*cantidad)++;

    return nuevo;
}

double calcular_monto_devolucion(const char *fecha_inicio, const char *fecha_entrega_esperada, const char *fecha_devolucion, int *entrega_tardia) {
    *entrega_tardia = 0;

    long dias_inicio = fecha_a_dias(fecha_inicio);
    long dias_esperada = fecha_a_dias(fecha_entrega_esperada);
    long dias_devolucion = fecha_a_dias(fecha_devolucion);

    if (dias_inicio < 0 || dias_esperada < 0 || dias_devolucion < 0) {
        return 0.0; /* fechas invalidas */
    }

    /* duracion real del prestamo de la fecha de inicio a la fecha real de devolucion */
    long duracion = dias_devolucion - dias_inicio;
    if (duracion < 1) {
        duracion = 1; /* se cobra al menos 1 dia */
    }

    /* dias de atraso lo que se paso de la fecha de entrega esperada */
    long dias_tarde = dias_devolucion - dias_esperada;
    if (dias_tarde < 0) {
        dias_tarde = 0;
    }
    *entrega_tardia = (dias_tarde > 0) ? 1 : 0;

    int tarifa_dia;
    int tarifa_tardia;
    if (duracion <= LIMITE_DIAS_TRAMO_1) {
        tarifa_dia = TARIFA_DIA_TRAMO_1;
        tarifa_tardia = TARIFA_TARDIA_TRAMO_1;
    } else if (duracion <= LIMITE_DIAS_TRAMO_2) {
        tarifa_dia = TARIFA_DIA_TRAMO_2;
        tarifa_tardia = TARIFA_TARDIA_TRAMO_2;
    } else {
        tarifa_dia = TARIFA_DIA_TRAMO_3;
        tarifa_tardia = TARIFA_TARDIA_TRAMO_3;
    }

    return (double)duracion * tarifa_dia + (double)dias_tarde * tarifa_tardia;
}

void mostrar_historial(const Prestamo *prestamos, int cantidad, const char *fecha_desde, const char *fecha_hasta, const Ejemplar *ejemplares, int cantidad_ejemplares) {
    long dias_desde = fecha_a_dias(fecha_desde);
    long dias_hasta = fecha_a_dias(fecha_hasta);

    if (dias_desde < 0 || dias_hasta < 0) {
        printf("Rango de fechas invalido.\n");
        return;
    }

    int mostrados = 0;
    for (int i = 0; i < cantidad; i++) {
        long dias_entrega = fecha_a_dias(prestamos[i].fecha_entrega_esperada);
        if (dias_entrega < dias_desde || dias_entrega > dias_hasta) {
            continue;
        }

        printf("- Prestamo #%d | usuario: %s | estado: %s\n",
               prestamos[i].id, prestamos[i].usuario_id, estado_a_texto(prestamos[i].estado));
        printf("  ejemplares: ");
        imprimir_ejemplares_prestamo(&prestamos[i], ejemplares, cantidad_ejemplares);
        printf("\n");
        if (prestamos[i].estado == PRESTAMO_FINALIZADO) {
            printf("  entrega tardia: %s\n", prestamos[i].entrega_tardia ? "si" : "no");
        }
        mostrados++;
    }

    if (mostrados == 0) {
        printf("No hay prestamos en el rango de fechas indicado.\n");
    }
}

void mostrar_vencimientos(const Prestamo *prestamos, int cantidad, const char *fecha_sistema, const Ejemplar *ejemplares, int cantidad_ejemplares) {
    long dias_hoy = fecha_a_dias(fecha_sistema);
    if (dias_hoy < 0) {
        printf("Fecha de sistema invalida.\n");
        return;
    }

    int mostrados = 0;
    for (int i = 0; i < cantidad; i++) {
        /* solo interesan los prestamos que aun no se han devuelto */
        if (prestamos[i].fecha_devolucion != NULL || prestamos[i].estado == PRESTAMO_FINALIZADO) {
            continue;
        }

        long dias_entrega = fecha_a_dias(prestamos[i].fecha_entrega_esperada);
        if (dias_entrega < 0) {
            continue;
        }

        long dias_restantes = dias_entrega - dias_hoy;
        if (dias_restantes > DIAS_PROXIMO_VENCIMIENTO) {
            continue; /* todavia falta mucho, no interesa mostrarlo aqui */
        }

        const char *estatus = (dias_restantes < 0) ? "vencido" : "proximo a vencer";

        printf("- Prestamo #%d | usuario: %s | fecha de entrega: %s | estatus: %s\n",
               prestamos[i].id, prestamos[i].usuario_id,
               prestamos[i].fecha_entrega_esperada, estatus);
        printf("  ejemplares: ");
        imprimir_ejemplares_prestamo(&prestamos[i], ejemplares, cantidad_ejemplares);
        printf("\n");
        mostrados++;
    }

    if (mostrados == 0) {
        printf("No hay prestamos vencidos ni proximos a vencer.\n");
    }
}

void liberar_prestamos(Prestamo *prestamos, int cantidad) {
    for (int i = 0; i < cantidad; i++) {
        free(prestamos[i].usuario_id);
        free(prestamos[i].fecha_inicio);
        free(prestamos[i].fecha_entrega_esperada);
        free(prestamos[i].fecha_devolucion);
        free(prestamos[i].ids_ejemplares);
    }
    free(prestamos);
}
