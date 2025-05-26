#pragma once
#include <raylib.h>
#include "configuracion.h"

#define TAM_BOTONES_PLAYER 50

#define ARCHIVOS_MAX_COLUMNA 3
#define ARCHIVOS_BOTON_ANCHO (PANTALLA_ANCHO/ARCHIVOS_MAX_COLUMNA)

struct Botones {
	Rectangle anterior;
	Rectangle play;
	Rectangle siguiente;
	Rectangle volumenSlider;
	Rectangle progresoSlider;
}botones;

struct VisualizacionPista {
	Rectangle caratulaArea;
	Rectangle fondoVista;
	Vector2 tituloPos, tituloPos2;
	Vector2 artistaPos, artistaPos2;
	Vector2 albumPos;
}vista;




