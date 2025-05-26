//Cargar playlist, guardar playlist, crear playlist, agregar imagen, estructura
#pragma once
#include <stdio.h>
#include "configuracion.h"

#define MAX_NOMBRE 12
#define MAX_RUTA_ARCHIVO_IMAGEN 128

typedef struct NodoPista {
    char ruta[MAX_RUTA_ARCHIVO_PISTA];
    struct NodoPista* siguiente;
    struct NodoPista* anterior;
}NodoPista;

typedef struct {
    int numeroDePistas;
    NodoPista* cabeza;
    NodoPista* final;
}Playlist;

void agregarPistaAlFinal(Playlist *playlist, const char *ruta) {
    NodoPista *nuevo = (NodoPista*) malloc(sizeof(NodoPista));
    strcpy(nuevo->ruta, ruta);
    nuevo->siguiente = NULL;

    if (playlist->cabeza == NULL) {
        nuevo->anterior = NULL;
        playlist->cabeza = nuevo;
    }else {
        NodoPista* temp = playlist->cabeza;
        while (temp->siguiente != NULL) {
            temp = temp->siguiente;
        }
        temp->siguiente = nuevo;
        nuevo->anterior = temp;
    }
    playlist->final = nuevo;
}

void agregarPistaAlInicio(Playlist* playlist, const char* ruta) {
    NodoPista* nuevo = (NodoPista*)malloc(sizeof(NodoPista));
    strcpy(nuevo->ruta, ruta);
    nuevo->anterior = NULL;
    nuevo->siguiente = playlist->cabeza;

    if (playlist->cabeza != NULL) {
        playlist->cabeza->anterior = nuevo;
    }

    playlist->cabeza = nuevo;
    playlist->numeroDePistas++;
}


