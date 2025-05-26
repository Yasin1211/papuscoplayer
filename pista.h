//Buscar pista y reproducir pista, metadatos con ID3v1Tag, estructuras
#pragma once
#include <stdio.h>
#include <math.h>
#include <string.h>
#define MAX_MINUTO_ETIQUETA 8
#define MAX_RUTA_ARCHIVO_PISTA 128
#define RUTA_MUSICA "Musica/"

typedef struct {
    //Esta estructura se guarda en las pistas con ID3v1Tag,
    //en los ultimos 128 bytes del archivo
    char tag[3];      // debe de guardar la palabra "TAG" para confirmar que contiene metadatos de tipo ID3v1Tag 
    char title[30];
    char artist[30];
    char album[30];
    char year[4];
    char comment[30];
    unsigned char genre;
} ID3v1Tag;

typedef struct {
    float minutos;
    float segundos;
}Duracion;

typedef struct {
    ID3v1Tag metadatos;
    char ruta[MAX_RUTA_ARCHIVO_PISTA];
    Music stream;
    char etiquetaTranscurrido[MAX_MINUTO_ETIQUETA], etiquetaTotal[MAX_MINUTO_ETIQUETA];
    Duracion tiempoTranscurrido, tiempoTotal;
    float segundosTranscurridos;
    int tieneMetadatos, estaPausada;
}Pista;

int obtenerMetadatos(ID3v1Tag* metadatosPista, const char* rutaPista) {
    FILE* archivo = fopen(rutaPista, "rb");
    if (!archivo) {
        perror("Error al abrir el archivo");
        return 0;
    }

    fseek(archivo, -128, SEEK_END); // Posicionarnos en los últimos 128 bytes
    if (fread(metadatosPista, sizeof(ID3v1Tag), 1, archivo) != 1) {
        fclose(archivo);
        return -1; // Error al leer los metadatos
    }

    fclose(archivo);

    if (strncmp(metadatosPista->tag, "TAG", 3) == 0) {
        return 1; // Metadatos encontrados
    }

    return -1; // No se encontró un ID3v1 válido
}

Duracion convertirSegundosAMinutosYSegundos(float segundos) {
    Duracion resultado;
    resultado.minutos = floor(segundos / 60);
    resultado.segundos = roundf(segundos - (60 * resultado.minutos));

    // si por redondeo segundos llega a 60 se ajusta
    if (resultado.segundos >= 60) {
        resultado.segundos = 0;
        resultado.minutos++;
    }

    return resultado;
}

