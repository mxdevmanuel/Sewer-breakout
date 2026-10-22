#pragma once

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include "manager.h"

class Engine
{
private:

	int fps; // Cuadros por segundo
	bool running; // Estado del juego
	bool fullScreen; // Bandera de pantalla completa
    bool error;
	int currentLevel;
	bool pause;

	ALLEGRO_DISPLAY *display; // Pantalla donde se dibujara el juego

	ALLEGRO_BITMAP *titleScreen; // Imagen de bienvenida al juego
	ALLEGRO_BITMAP *gameover; // Imagen de fin de juego
    ALLEGRO_SAMPLE *intro, *spit, *roaring;

public:
    Manager *TheManager = Manager::Instance(); // Administrador de objetos en el juego

    ALLEGRO_FONT *billboardFont;
	ALLEGRO_TIMER *timer; // Temporizador de frecuencia de dibujado
	ALLEGRO_EVENT_QUEUE *event_list; // Cola de eventos producidos en el juego
	ALLEGRO_EVENT ev{}; // Variable que recibe los eventos del juego
	bool redraw; // Variable que indica cuando debe ser redibujada la pantalla

	Engine(int width, int height); // Constructor
	~Engine(); // Destructor

	bool isRunning(); // Devuelve el estado del juego
	void stopRunning(); // Marca el juego como terminado

	void showTitleScreen(); // Muestra la imagen de bienvenida en pantalla
	void showgameover(); // Muestra la imagen del fin de juego en la pantalla
    void showCredits(); // Muestra los creditos finales
	void showPause();

	void handleEvents(); // Metodo sin retorno que monitorea los eventos del juego
	void update(); // Metodo sin retorno que actualiza los objetos del juego
	void draw(); // Metodo sin retorno que dibuja el juego
	void defineMaps(); // Metodo que define los mapas a utilizar en el juego
	void createHero(); // Metodo que inicializa al heroe dentro del manager
	void keyPressed(int key); // Metodo sin retorno que maneja las teclas presionadas
    void keyReleased(int key);
	void setFirstLevel();
	void setSecondLevel();
	void setThirdLevel();
	void selectLevel(int level);
    void shoot();

	ALLEGRO_DISPLAY *getDisplay(); // Devuelve la pantalla actual

};

