#ifndef MENU_H
#define MENU_H

/* punto de entrada del ciclo de menu principal, corre hasta que el usuario elige salir */
void ejecutar_menu_principal(void);

/* submenu de opciones operativas (catalogo, usuarios, historial, vencimientos, estadisticas) */
void ejecutar_menu_operativo(void);

/* submenu de opciones generales (busquedas, prestamo, devolucion) */
void ejecutar_menu_general(void);

#endif /* MENU_H */
