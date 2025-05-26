//Cargar y guardar la configuracion del usuario + estructura
#pragma once
#include <stdio.h>
#include "pista.h"
#include "playlist.h"
#define RUTA_PERFIL "Usuario/perfil.dat"

#define PANTALLA_FRAMES 60
#define PANTALLA_ANCHO 900
#define PANTALLA_LARGO 800
#define PANTALLA_TITULO "papuscoplayer"

#define RUTA_FONDO "Usuario/fondo.png"
#define RUTA_FONT "Usuario/popstar.ttf"

#define TAM_FONT (PANTALLA_LARGO/35)
#define TAM_CARATULA 350

typedef struct {
	char ultimaPistaGuardada[MAX_RUTA_ARCHIVO_PISTA];
	float volumenGeneral, tiempoGuardado;
	int numeroDeListas;
}Perfil;

int cargarPerfil(Perfil *perfil){
	FILE* archivo = fopen(RUTA_PERFIL, "rb");
	if (archivo) {
		fread(&perfil->ultimaPistaGuardada, MAX_RUTA_ARCHIVO_PISTA, 1, archivo);
		fread(&perfil->volumenGeneral, sizeof(float), 1, archivo);
		fread(&perfil->tiempoGuardado, sizeof(float), 1, archivo);
		//...
		fclose(archivo);
		return 1;
	}
	return 0;
}

int guardarPerfil(Perfil* perfil) {
	FILE* archivo = fopen(RUTA_PERFIL, "wb");
	if (archivo) {
		fwrite(&perfil->ultimaPistaGuardada, MAX_RUTA_ARCHIVO_PISTA, 1, archivo);
		fwrite(&perfil->volumenGeneral, sizeof(float), 1, archivo);
		fwrite(&perfil->tiempoGuardado, sizeof(float), 1, archivo);
		//...
		fclose(archivo);
		return 1;
	}
	return 0;
}


void liberarPlaylist(Playlist* playlist) {
	if (!playlist || !playlist->cabeza) {//si la lista ya no apunta nada
		return;
	}
	NodoPista* temp = playlist->cabeza, * siguiente = NULL;
	while (temp != NULL) {
		siguiente = temp->siguiente; //guardamos la siguiente en una temporal
		free(temp);//lliberamos la actual
		temp = siguiente;//ahora usamos el valor de la temporal para podwer continuar con los demas nodos
	}
	playlist->numeroDePistas = 0;
	playlist->cabeza = NULL;
	playlist->final = NULL;
}
