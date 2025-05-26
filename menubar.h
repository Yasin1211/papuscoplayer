#pragma once
#include <raylib.h>
#define TAM_MENU_BARRA (10 + TAM_FONT)

#define TAM_MENU_BOTONES (PANTALLA_ANCHO / 3)

#define MAX_PISTAS_VISIBLES 15
#define ANCHO_SLIDER 15

typedef enum {
	REPRODUCTOR,
	MUSICA,
	VER_COLA,
	SALIR,
}EstadoBarraMenu;


typedef enum {
	VER_LISTA_NORMAL,
	VER_LISTA_AGREGAR_CANCION
}Modo;

struct MenuBarra{
	Rectangle BotonMusica;
	Rectangle BotonListas;
	Rectangle BotonSalir;
}menuBarra;





