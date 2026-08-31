# Proyecto Programado #1 — Gestión de Biblioteca

**Curso:** Lenguajes de Programación — ITCR, Semestre II 2026
**Profesor:** Allan Rodríguez Dávila
**Equipo:** grupo de 2 personas
**Inicio:** domingo 30 de agosto 2026
**Entrega límite:** domingo 13 de septiembre 2026, 11:55:55 PM

---

## 1. Enunciado del proyecto

### Descripción general
Sistema de gestión de biblioteca en **C**, sin interfaz gráfica (consola). La información se almacena y consulta desde archivos **JSON** (cJSON o json-c).

El programa despliega un menú con dos submenús (Opciones Operativas y Opciones Generales). El menú debe reaparecer después de cada acción (excepto al salir) y cada submenú debe tener opción de volver al menú principal.

### 1.1 Opciones Operativas

**Gestión de catálogo**
- Mantenimiento del catálogo: incluir en lote (solo registros nuevos y con información correcta).
- Campos: nombre, autor, año publicación, género, resumen, cantidad. Unicidad por **nombre**.
- Archivo de entrada con formato delimitado por `#`:
  ```
  Cien años de soledad#Gabriel García Márquez#1967#Realismo mágico#Esta novela cuenta la historia#4
  1984#George Orwell#1949#Distopía#En un futuro totalitario, el protagonista lucha contra el régimen#3
  ```
- Según la cantidad indicada se genera un identificador por cada ejemplar (se almacenan ejemplares individuales en otro JSON).
- Mostrar reporte de registros no procesados.
- Ver catálogo completo con toda su información.

**Gestión de usuarios**
- CRUD: crear, ver, modificar, eliminar.
- Campos: número de identificación, nombre, dirección. Unicidad por **número de identificación**.
- No se puede eliminar un usuario con registros asociados (préstamos).

**Historial de préstamos**
- Mostrar préstamos en un rango de fechas (inicio–fin, según fecha de entrega).
- Información: id de préstamo, usuario, estado (activo / vencido / finalizado), nombre e id de ejemplares, si hubo entrega tardía (sí/no, solo si finalizado).

**Vencimiento de préstamos**
- Mostrar préstamos vencidos y próximos a vencer (0 a 5 días).
- Información: id de préstamo, usuario, fecha de entrega, estatus (vencido, próximo a vencer), nombre e id de ejemplares.
- Se calcula según la fecha de sistema.

**Estadísticas**
- A. Top 3 producciones más prestadas (con cantidad).
- B. Top 3 usuarios con más préstamos (con cantidad).
- C. Top 5 mes-año con mayor monto recaudado (según fecha de inicio).
- D. Género con más y con menos préstamos (solo géneros con préstamos generados).

### 1.2 Opciones Generales

**Búsqueda simple**
- Técnica "contiene" + "o": busca el texto en nombre, autor o resumen.
- Muestra por match: id, nombre, resumen, estado (disponible o no).

**Búsqueda avanzada**
- Campos: nombre, autor, género, resumen (cada uno opcional, vacío = no buscar).
- Por campo: técnica "contiene" o "exacta".
- Operador global entre campos: "o" / "y".
- Muestra por match: id, nombre, resumen.

**Préstamo de ejemplares**
- Usuario indica: usuario, fecha inicio, fecha final, ids de ejemplares deseados.
- Verifica disponibilidad de cada ejemplar en esas fechas.
- Genera comprobante: id préstamo, usuario, fecha inicio, fecha entrega, nombre e id de ejemplares disponibles.
- Requiere gestión de stock de ejemplares.

**Devolución de ejemplar**
- Solicita id de préstamo y fecha de devolución.
- Calcula el monto según tabla de tarifas (ver abajo).
- Muestra comprobante con el monto a cancelar.
- Requiere gestión de estados de ejemplares.

### Tabla de tarifas (devolución)

| Duración préstamo | Tarifa día préstamo | Tarifa día tardía |
|-------------------|---------------------|-------------------|
| 1 a 7 días        | ₡175                | ₡100              |
| 8 a 15 días       | ₡150                | ₡75               |
| 16 días o más     | ₡100                | ₡50               |
