#define _CRT_SECURE_NO_WARNINGS
#define RAYGUI_IMPLEMENTATION
#include <raylib.h>
#include <raygui.h>
#include <stdio.h>
#include "configuracion.h"
#include "pista.h"
#include "playlist.h"
#include "player.h"
#include "menubar.h"

//globales
struct PapuscoPlayerPublicas {
	FilePathList bibliotecaMusica; 
	Playlist cola;
	Pista pistaActual;
	Font font;
	Perfil perfil;
	Texture2D fondo, caratula;
	EstadoBarraMenu seleccionado;
	Modo listaModo;
	Rectangle menuVentana;
	char etiquetaVolumen[6]; // etiqueta volumen
}papuscoPlayer;

float offsetScroll = 0.0f;
float offsetScrollCola = 0.0f;
int ventanaDeIngresoDeTexto = 0;

//funciones prototipos
void iniciarPapuscoPlayer();
void cerrarPapuscoPlayer();
void reproductor();
void reproducirPista(const char*);
void mostrarCarpetaDeMusica();
void cargarCarpetaDeMusica();
void mostrarCola();
void menu();

void iniciarPapuscoPlayer() {
	Image tempFondo;//temporales, se convierten en texturas y se dejan de usar
	//Arrancamos raylib y todo lo que necesitemos
	InitWindow(PANTALLA_ANCHO, PANTALLA_LARGO, PANTALLA_TITULO);
	InitAudioDevice();
	SetTargetFPS(PANTALLA_FRAMES);
	//SetExitKey(KEY_ESCAPE);

	//Cargar datos
	if (!cargarPerfil(&papuscoPlayer.perfil)) {
		papuscoPlayer.perfil.volumenGeneral = 1.0f;
		guardarPerfil(&papuscoPlayer.perfil);
	}
	else {
		if (IsPathFile(papuscoPlayer.perfil.ultimaPistaGuardada)) {
			reproducirPista(papuscoPlayer.perfil.ultimaPistaGuardada);
			SeekMusicStream(papuscoPlayer.pistaActual.stream, papuscoPlayer.perfil.tiempoGuardado);
			PauseMusicStream(papuscoPlayer.pistaActual.stream);
		}
	}
	
	papuscoPlayer.pistaActual.estaPausada = 1;

	//Cargar imagenes 
	tempFondo = LoadImage(RUTA_FONDO); //cargamos la imagen
	ImageResize(&tempFondo, PANTALLA_ANCHO, PANTALLA_LARGO); //reescalamos la imagen
	papuscoPlayer.fondo = LoadTextureFromImage(tempFondo); //la convertimos en textura
	UnloadImage(tempFondo); //liberamos la imagen

	//Cargar font      
	papuscoPlayer.font = LoadFontEx(RUTA_FONT, TAM_FONT, NULL, 0);
	GuiSetFont(papuscoPlayer.font);
	
	GuiSetStyle(DEFAULT, TEXT_SIZE, TAM_FONT);

	//Controles del reproductor posicion y/o area
	botones.play = (Rectangle){ (PANTALLA_ANCHO / 2) - (TAM_BOTONES_PLAYER / 2), PANTALLA_LARGO - TAM_BOTONES_PLAYER - 20, TAM_BOTONES_PLAYER, TAM_BOTONES_PLAYER };
	botones.siguiente = (Rectangle){ botones.play.x + botones.play.width + 30, PANTALLA_LARGO - TAM_BOTONES_PLAYER - 20, TAM_BOTONES_PLAYER, TAM_BOTONES_PLAYER };
	botones.anterior = (Rectangle){ botones.play.x - botones.play.width - 30, PANTALLA_LARGO - TAM_BOTONES_PLAYER - 20, TAM_BOTONES_PLAYER, TAM_BOTONES_PLAYER };
	botones.volumenSlider = (Rectangle){30, PANTALLA_LARGO - TAM_BOTONES_PLAYER, TAM_BOTONES_PLAYER*5, 10};
	botones.progresoSlider = (Rectangle){100, botones.play.y - 100, PANTALLA_ANCHO - (100*2), 10};
	vista.tituloPos2 = (Vector2){botones.siguiente.x + botones.siguiente.width + 30, botones.siguiente.y};
	vista.artistaPos2 = (Vector2){vista.tituloPos2.x, vista.tituloPos2.y + TAM_FONT + 5};
	
	//Informacion de la pista (Vista) posicion y area
	vista.caratulaArea = (Rectangle){30, TAM_MENU_BARRA + 180, TAM_CARATULA, TAM_CARATULA };
	vista.tituloPos = (Vector2){vista.caratulaArea.x, vista.caratulaArea.y + 30};
	vista.artistaPos = (Vector2){vista.tituloPos.x, vista.tituloPos.y + TAM_FONT + 30 };
	vista.albumPos = (Vector2){ vista.tituloPos.x, vista.artistaPos.y + TAM_FONT + 30 };
	vista.fondoVista = (Rectangle){ 0, vista.caratulaArea.y - TAM_FONT - 20, PANTALLA_ANCHO, vista.caratulaArea.height + TAM_FONT + 60 };
	
	//Menu (barra superior)
	menuBarra.BotonMusica = (Rectangle){0, 0, TAM_MENU_BOTONES, TAM_MENU_BARRA};
	menuBarra.BotonListas = (Rectangle){menuBarra.BotonMusica.x + TAM_MENU_BOTONES, 0, TAM_MENU_BOTONES, TAM_MENU_BARRA };
	//menuBarra.BotonConfiguracion = (Rectangle){ menuBarra.BotonListas.x + TAM_MENU_BOTONES, 0, TAM_MENU_BOTONES, TAM_MENU_BARRA };
	menuBarra.BotonSalir = (Rectangle){ menuBarra.BotonListas.x + TAM_MENU_BOTONES, 0, TAM_MENU_BOTONES, TAM_MENU_BARRA };
	papuscoPlayer.menuVentana = (Rectangle){ 0, TAM_MENU_BARRA, PANTALLA_ANCHO, vista.fondoVista.y + vista.fondoVista.height - TAM_MENU_BARRA };
	
	//Cola
	papuscoPlayer.cola.cabeza = NULL;
	papuscoPlayer.cola.numeroDePistas = 0;
}

//void 
NodoPista* nodoActual = NULL;
void reproductor() {
	
	float temp, temp2;
	UpdateMusicStream(papuscoPlayer.pistaActual.stream);
	
	DrawTexture(papuscoPlayer.fondo, 0, TAM_MENU_BARRA, WHITE);
	DrawRectangle(0, 0, PANTALLA_ANCHO, TAM_MENU_BARRA, RAYWHITE);

	//Tiempo Transcurrido y Total de la pista (etiquetas)
	papuscoPlayer.pistaActual.segundosTranscurridos = GetMusicTimePlayed(papuscoPlayer.pistaActual.stream);
	papuscoPlayer.pistaActual.tiempoTranscurrido = convertirSegundosAMinutosYSegundos(papuscoPlayer.pistaActual.segundosTranscurridos);
	snprintf(papuscoPlayer.pistaActual.etiquetaTranscurrido, MAX_MINUTO_ETIQUETA, "%.0f:%02.0f", papuscoPlayer.pistaActual.tiempoTranscurrido.minutos, papuscoPlayer.pistaActual.tiempoTranscurrido.segundos);
	//Progreso de la cancion (interactuable)
	temp = papuscoPlayer.pistaActual.segundosTranscurridos;
	GuiSlider(botones.progresoSlider, papuscoPlayer.pistaActual.etiquetaTranscurrido, papuscoPlayer.pistaActual.etiquetaTotal, &papuscoPlayer.pistaActual.segundosTranscurridos, 0.0, GetMusicTimeLength(papuscoPlayer.pistaActual.stream));
	if (temp != papuscoPlayer.pistaActual.segundosTranscurridos && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		SeekMusicStream(papuscoPlayer.pistaActual.stream, papuscoPlayer.pistaActual.segundosTranscurridos);
	}
	//Slider del volumen
	temp2 = papuscoPlayer.perfil.volumenGeneral;
	snprintf(papuscoPlayer.etiquetaVolumen, 4, "%.0f", papuscoPlayer.perfil.volumenGeneral * 100);
	GuiSlider(botones.volumenSlider, "", papuscoPlayer.etiquetaVolumen, &papuscoPlayer.perfil.volumenGeneral, 0.0f, 1.0f);
	if (temp2 != papuscoPlayer.perfil.volumenGeneral) {
		SetMusicVolume(papuscoPlayer.pistaActual.stream, papuscoPlayer.perfil.volumenGeneral);
	}

	//DrawRectangleRec(vista.fondoVista, RAYWHITE);

	//DrawRectangleRec(vista.caratulaArea, LIGHTGRAY);
	DrawTextEx(papuscoPlayer.font, papuscoPlayer.pistaActual.metadatos.title, vista.tituloPos, TAM_FONT, 1, BLACK);
	DrawTextEx(papuscoPlayer.font, papuscoPlayer.pistaActual.metadatos.artist, vista.artistaPos, TAM_FONT - 1, 1, BLACK);
	DrawTextEx(papuscoPlayer.font, papuscoPlayer.pistaActual.metadatos.album, vista.albumPos, TAM_FONT - 2, 1, BLACK);
	
	if (IsPathFile(papuscoPlayer.pistaActual.ruta)) {
		if (GuiButton(botones.play, papuscoPlayer.pistaActual.estaPausada ? "|>" : "||") || IsKeyPressed(KEY_SPACE)) {
			if (papuscoPlayer.pistaActual.estaPausada) ResumeMusicStream(papuscoPlayer.pistaActual.stream);
			else PauseMusicStream(papuscoPlayer.pistaActual.stream);
			papuscoPlayer.pistaActual.estaPausada = !papuscoPlayer.pistaActual.estaPausada;
		}
		if (GuiButton(botones.siguiente, "|>|") || (IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_RIGHT))) {
			if (nodoActual != NULL && papuscoPlayer.cola.numeroDePistas > 0) {
				if (nodoActual->siguiente == NULL) {
					nodoActual = papuscoPlayer.cola.cabeza;
				}
				else {
					nodoActual = nodoActual->siguiente;		
				}
				reproducirPista(nodoActual->ruta);	
			}
			
		}
		if (GuiButton(botones.anterior, "|<|") || (IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_LEFT))) {
			if (papuscoPlayer.pistaActual.segundosTranscurridos > 2) {
				StopMusicStream(papuscoPlayer.pistaActual.stream);
				PlayMusicStream(papuscoPlayer.pistaActual.stream);
			}else if (nodoActual != NULL && papuscoPlayer.cola.numeroDePistas > 0) {
				if (nodoActual->anterior == NULL) {
					nodoActual = papuscoPlayer.cola.final;
				}
				else {
					nodoActual = nodoActual->anterior;
				}
				reproducirPista(nodoActual->ruta);
			}
		}
		if (floor(GetMusicTimeLength(papuscoPlayer.pistaActual.stream)) <= floor(papuscoPlayer.pistaActual.segundosTranscurridos)) {
			if (nodoActual != NULL && papuscoPlayer.cola.numeroDePistas > 0) {
				if (nodoActual->siguiente == NULL) {
					nodoActual = papuscoPlayer.cola.cabeza;
				}
				else {
					nodoActual = nodoActual->siguiente;
				}
				reproducirPista(nodoActual->ruta);
			}
		}
	}
	
}



void menu() {
	//Botones
	if (GuiButton(menuBarra.BotonMusica, "Musica")) {
		if (papuscoPlayer.seleccionado == MUSICA) {
			papuscoPlayer.seleccionado = REPRODUCTOR;
		}
		else {
			cargarCarpetaDeMusica();
			papuscoPlayer.seleccionado = MUSICA;
		}
	}
	if (GuiButton(menuBarra.BotonListas, "Ver Cola")) {
		if (papuscoPlayer.seleccionado == VER_COLA) {
			papuscoPlayer.seleccionado = REPRODUCTOR;
		}
		else {
			papuscoPlayer.seleccionado = VER_COLA;
		}
	}
	if (GuiButton(menuBarra.BotonSalir, "Salir")) {
		papuscoPlayer.seleccionado = SALIR;
	}
	//Ventanas
	switch (papuscoPlayer.seleccionado) {
		case REPRODUCTOR:
			break;
		case MUSICA:
			mostrarCarpetaDeMusica();
			break;
		case VER_COLA:
			mostrarCola();
			break;
		case SALIR:
			break;
		default:
			break;
	}
}

//Funcion MAIN
int main() {

	iniciarPapuscoPlayer(); //cargamos todo

	while (!WindowShouldClose() && papuscoPlayer.seleccionado != SALIR) {
		BeginDrawing(); 
		{
			ClearBackground(BLACK);
			reproductor();
			menu();
		}
		EndDrawing();
	}

	cerrarPapuscoPlayer();

	return 0;
}

void reproducirPista(const char* ruta) {
	if(IsPathFile(ruta)){
		StopMusicStream(papuscoPlayer.pistaActual.stream);
		strcpy(papuscoPlayer.pistaActual.ruta, ruta);
		papuscoPlayer.pistaActual.stream = LoadMusicStream(papuscoPlayer.pistaActual.ruta);
		papuscoPlayer.pistaActual.tieneMetadatos = obtenerMetadatos(&papuscoPlayer.pistaActual.metadatos, papuscoPlayer.pistaActual.ruta);
		papuscoPlayer.pistaActual.metadatos.title[23] = '\0';
		papuscoPlayer.pistaActual.metadatos.artist[23] = '\0';
		papuscoPlayer.pistaActual.metadatos.album[23] = '\0';
		printf("(papuscoplayer): %d\n", papuscoPlayer.pistaActual.tieneMetadatos);
		if (papuscoPlayer.pistaActual.tieneMetadatos == -1 || papuscoPlayer.pistaActual.tieneMetadatos == 0) {
			strcpy(papuscoPlayer.pistaActual.metadatos.artist, "Artista Desconocido");
			strcpy(papuscoPlayer.pistaActual.metadatos.album, " ");
		}
		if (strlen(papuscoPlayer.pistaActual.metadatos.title) <= 0 || papuscoPlayer.pistaActual.tieneMetadatos == -1 || papuscoPlayer.pistaActual.tieneMetadatos == 0) {
			strcpy(papuscoPlayer.pistaActual.metadatos.title, GetFileNameWithoutExt(papuscoPlayer.pistaActual.ruta));
		}
		papuscoPlayer.pistaActual.tiempoTotal = convertirSegundosAMinutosYSegundos(GetMusicTimeLength(papuscoPlayer.pistaActual.stream));
		snprintf(papuscoPlayer.pistaActual.etiquetaTotal, MAX_MINUTO_ETIQUETA, "%.0f:%02.0f", papuscoPlayer.pistaActual.tiempoTotal.minutos, papuscoPlayer.pistaActual.tiempoTotal.segundos);
		PlayMusicStream(papuscoPlayer.pistaActual.stream);
		SetMusicVolume(papuscoPlayer.pistaActual.stream, papuscoPlayer.perfil.volumenGeneral);
		papuscoPlayer.pistaActual.estaPausada = 0;
	}
}

void mostrarCola() {
	NodoPista* aux = papuscoPlayer.cola.cabeza;
	float x = (2 * TAM_BOTONES_PLAYER) + 5, y = TAM_MENU_BARRA + 5;
	int indice = (int)offsetScrollCola;
	int pistasMax = papuscoPlayer.cola.numeroDePistas < MAX_PISTAS_VISIBLES ? papuscoPlayer.cola.numeroDePistas : MAX_PISTAS_VISIBLES;

	DrawRectangleRec(papuscoPlayer.menuVentana, RAYWHITE);
	//DrawText("Musica seleccionado", 0, TAM_FONT, TAM_FONT, RED);

	DrawTextEx(papuscoPlayer.font, GetFileNameWithoutExt(papuscoPlayer.pistaActual.ruta), vista.tituloPos2, (TAM_FONT / 2), 1, GRAY);
	DrawTextEx(papuscoPlayer.font, papuscoPlayer.pistaActual.metadatos.artist, vista.artistaPos2, (TAM_FONT / 2) - 1, 1, GRAY);

	if (papuscoPlayer.cola.numeroDePistas <= 0) {
		DrawTextEx(papuscoPlayer.font, "No se ha agregado ninguna pista a la cola", (Vector2) { 5, y }, TAM_FONT, 1, BLACK);
		return;
	}
	if(GuiButton((Rectangle){PANTALLA_ANCHO - 300, y, 200, TAM_MENU_BARRA}, "Vaciar cola")) {
		liberarPlaylist(&papuscoPlayer.cola);
	}
	char etiquetaCola[16], num[8];
	snprintf(etiquetaCola, sizeof(etiquetaCola), "Cola (%d)", papuscoPlayer.cola.numeroDePistas);
	DrawTextEx(papuscoPlayer.font, etiquetaCola, (Vector2) { 5, y }, TAM_FONT, 1, BLACK);
	y += TAM_FONT + 10;
	for (int i = indice; i < (indice + pistasMax) && i < papuscoPlayer.cola.numeroDePistas; i++) {
		snprintf(num, sizeof(num), "(%d)", i+1);
		DrawTextEx(papuscoPlayer.font, num, (Vector2) { 5, y }, TAM_FONT, 1, GRAY);
		DrawTextEx(papuscoPlayer.font, GetFileNameWithoutExt(aux->ruta), (Vector2) { x, y }, TAM_FONT, 1, LIGHTGRAY);
		
		y += TAM_FONT + 10;
		Rectangle scrollbarRect = {
			PANTALLA_ANCHO - ANCHO_SLIDER,
			papuscoPlayer.menuVentana.y + TAM_FONT + 10,
			ANCHO_SLIDER,
			pistasMax * (TAM_FONT + 10)
		};

		//Slider
		offsetScroll = GuiScrollBar(scrollbarRect, offsetScroll, 0, papuscoPlayer.cola.numeroDePistas - MAX_PISTAS_VISIBLES);
		//siguiente ruta
		aux = aux->siguiente;
	}
}

void mostrarCarpetaDeMusica() {
	float x = 5 + (2*TAM_BOTONES_PLAYER) + 5, y = TAM_MENU_BARRA + 5;
	int indice = (int)offsetScroll;
	int pistasMax = papuscoPlayer.bibliotecaMusica.count < MAX_PISTAS_VISIBLES ? papuscoPlayer.bibliotecaMusica.count : MAX_PISTAS_VISIBLES; 

	DrawRectangleRec(papuscoPlayer.menuVentana, RAYWHITE);
	//DrawText("Musica seleccionado", 0, TAM_FONT, TAM_FONT, RED);

	DrawTextEx(papuscoPlayer.font, GetFileNameWithoutExt(papuscoPlayer.pistaActual.ruta), vista.tituloPos2, (TAM_FONT / 2), 1, GRAY);
	DrawTextEx(papuscoPlayer.font, papuscoPlayer.pistaActual.metadatos.artist, vista.artistaPos2, (TAM_FONT / 2) - 1, 1, GRAY);

	if (papuscoPlayer.bibliotecaMusica.count <= 0) {
		DrawTextEx(papuscoPlayer.font, "No se encontro ningun archivo MP3", (Vector2) { 5, y }, TAM_FONT, 1, BLACK);
		return;
	}
	

	DrawTextEx(papuscoPlayer.font, "Pistas encontradas", (Vector2){5, y}, TAM_FONT, 1, BLACK);
	y += TAM_FONT + 10;
	for (int i = indice; i < (indice + pistasMax) && i < papuscoPlayer.bibliotecaMusica.count; i++) {
		DrawTextEx(papuscoPlayer.font, GetFileNameWithoutExt(papuscoPlayer.bibliotecaMusica.paths[i]), (Vector2) {x, y}, TAM_FONT, 1, LIGHTGRAY);
		//Reproducir
		if (GuiButton((Rectangle) { 5 + TAM_BOTONES_PLAYER, y, TAM_BOTONES_PLAYER, TAM_FONT + 10 }, "|>")) {
			reproducirPista(papuscoPlayer.bibliotecaMusica.paths[i]);
			return;
		}
		//Agregar a Playlist
		if (GuiButton((Rectangle) { 5, y, TAM_BOTONES_PLAYER, TAM_FONT + 10 }, "+")) {
			printf("\n(papuscoPlayer) Agregar a cola\n");
			agregarPistaAlFinal(&papuscoPlayer.cola, papuscoPlayer.bibliotecaMusica.paths[i]);
			nodoActual = papuscoPlayer.cola.cabeza;
			if(papuscoPlayer.cola.numeroDePistas <= 0 && papuscoPlayer.pistaActual.estaPausada)
				reproducirPista(nodoActual->ruta);
			papuscoPlayer.cola.numeroDePistas++;
			return;
		}
		y += TAM_FONT + 10;
		Rectangle scrollbarRect = {
			PANTALLA_ANCHO - ANCHO_SLIDER,
			papuscoPlayer.menuVentana.y + TAM_FONT + 10,
			ANCHO_SLIDER,
			pistasMax * (TAM_FONT + 10)
		};

		//Slider
		offsetScroll = GuiScrollBar(scrollbarRect, offsetScroll, 0, papuscoPlayer.bibliotecaMusica.count - MAX_PISTAS_VISIBLES);
		
	}
}

void cargarCarpetaDeMusica() {
	if (papuscoPlayer.bibliotecaMusica.count > 0) {
		UnloadDirectoryFiles(papuscoPlayer.bibliotecaMusica);
	}
	papuscoPlayer.bibliotecaMusica = LoadDirectoryFilesEx(RUTA_MUSICA, ".mp3", true);
}

void cerrarPapuscoPlayer() {
	strcpy(papuscoPlayer.perfil.ultimaPistaGuardada, papuscoPlayer.pistaActual.ruta);
	papuscoPlayer.perfil.tiempoGuardado = papuscoPlayer.pistaActual.segundosTranscurridos;
	if (guardarPerfil(&papuscoPlayer.perfil)) {
		printf("\n(papuscoplayer) Perfil guardado exitosamente\n");
	}
	UnloadMusicStream(papuscoPlayer.pistaActual.stream);
	UnloadTexture(papuscoPlayer.fondo);
	UnloadFont(papuscoPlayer.font);
	liberarPlaylist(&papuscoPlayer.cola);
	UnloadDirectoryFiles(papuscoPlayer.bibliotecaMusica);
	CloseAudioDevice();
	CloseWindow();
}