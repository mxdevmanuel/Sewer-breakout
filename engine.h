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
#include "level.h"
#include <vector>

// Pantallas por las que pasa el juego
enum class GameState { Title, Playing, Paused, GameOver, Credits };

class Engine
{
private:

	int fps; // Cuadros por segundo
	bool running; // Estado del juego
	bool fullScreen; // Bandera de pantalla completa
    bool error;
	int currentLevel;
	std::vector<LevelData> levels; // Niveles leidos de levels/
	GameState state = GameState::Title; // Pantalla actual del juego
	int creditsTicks = 0; // Ticks transcurridos en los creditos
	bool won = false; // Se completo el ultimo nivel
	static constexpr int creditsPageTicks = 120; // Duracion de cada pagina de creditos (2 s a 60 fps)
	static constexpr int creditsPages = 3;

	ALLEGRO_DISPLAY *display = nullptr; // Pantalla donde se dibujara el juego

	ALLEGRO_BITMAP *titleScreen = nullptr; // Imagen de bienvenida al juego
	ALLEGRO_BITMAP *gameover = nullptr; // Imagen de fin de juego
    ALLEGRO_SAMPLE *intro = nullptr, *spit = nullptr, *roaring = nullptr;

    void fail(const char *heading, const char *message); // Muestra un error de inicializacion y detiene el juego

public:
    Manager *TheManager = Manager::Instance(); // Administrador de objetos en el juego

    ALLEGRO_FONT *billboardFont = nullptr;
	ALLEGRO_TIMER *timer = nullptr; // Temporizador de frecuencia de dibujado
	ALLEGRO_EVENT_QUEUE *event_list = nullptr; // Cola de eventos producidos en el juego
	ALLEGRO_EVENT ev{}; // Variable que recibe los eventos del juego
	bool redraw; // Variable que indica cuando debe ser redibujada la pantalla

	Engine(int width, int height); // Constructor
	~Engine(); // Destructor

	bool isRunning(); // Devuelve el estado del juego
	void stopRunning(); // Marca el juego como terminado

	void showTitleScreen(); // Muestra la imagen de bienvenida en pantalla
	void showgameover(); // Muestra la imagen del fin de juego en la pantalla
	void startCredits(); // Pasa a los creditos finales
	void drawCreditsPage(int page); // Dibuja una pagina de los creditos finales
	void showPause();
	void drawScene(); // Dibuja el nivel actual

	void handleEvents(); // Metodo sin retorno que monitorea los eventos del juego
	void update(); // Metodo sin retorno que actualiza los objetos del juego
	void updatePlaying(); // Actualiza el nivel mientras se juega
	void draw(); // Metodo sin retorno que dibuja el juego
	void loadLevels(); // Lee los niveles de levels/ y carga sus mapas
	void createHero(); // Metodo que inicializa al heroe dentro del manager
	void keyPressed(int key); // Metodo sin retorno que maneja las teclas presionadas
    void keyReleased(int key);
	void selectLevel(int level);
    void shoot();

	ALLEGRO_DISPLAY *getDisplay(); // Devuelve la pantalla actual

};

