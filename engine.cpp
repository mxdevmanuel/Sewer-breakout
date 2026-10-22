#include "engine.h"
#include "boss.h"
#include "enemy.h"
#include "hero.h"
#include "mapa.h"
#include "missile.h"
#include "item.h"
#include "enemymissile.h"

Engine::Engine(int width, int height)
{
    // Inicializar variables
    this->running = true;
    this->fps = 60;
    this->redraw = false;
    this->fullScreen = false;
    this->error = false;
	this->currentLevel = -1;
	this->pause = false;

    // Inicialiazar allegro y manejar errores de inicializacion
    if (!al_init()) {
        al_show_native_message_box(nullptr, "Error!", "Fallo inicialización", "Hubo un error al intentar inicializar Allegro", nullptr, ALLEGRO_MESSAGEBOX_ERROR);
        this->error = true;
    }

    // Crear timer
    timer = al_create_timer(1.0 / fps);
    if (!timer) //Manejar errores del timer
    {
        al_show_native_message_box(nullptr, "Error!", "Fallo inicialización", "Hubo un error al intentar inicializar el timer", nullptr, ALLEGRO_MESSAGEBOX_ERROR);
        this->error = true;
    }

    // Crear display
    display = al_create_display(width, height);
    if (!display) // Manejar errores del display
    {
        al_show_native_message_box(nullptr, "Error!", "Fallo inicialización", "Hubo un error al intentar inicializar el display", nullptr, ALLEGRO_MESSAGEBOX_ERROR);
        al_destroy_timer(timer);
        this->error = true;
    }

    // Crear cola de eventos
    event_list = al_create_event_queue();
    if (!event_list) // Manejar errores de la cola de eventos
    {
        al_show_native_message_box(display, "Error!", "Fallo inicialización", "Hubo un error al intentar inicializar la cola de eventos", nullptr, ALLEGRO_MESSAGEBOX_ERROR);
        al_destroy_timer(timer);
        al_destroy_display(display);
        this->error = true;
    }

    // Inicializar teclado y manejar errores
    if (!al_install_keyboard()) {
        al_show_native_message_box(display, "Error!", "Fallo inicialización", "Hubo un error al intentar inicializar el teclado", nullptr, ALLEGRO_MESSAGEBOX_ERROR);
        al_destroy_timer(timer);
        al_destroy_display(display);
        al_destroy_event_queue(event_list);
        this->error = true;
    }

    // Inicializar addon de imagenes y manejar errores
    if (!al_init_image_addon()) {
        al_show_native_message_box(nullptr, "Error!", "Fallo inicialización", "Hubo un error al intentar inicializar el addon de imagenes", nullptr, ALLEGRO_MESSAGEBOX_ERROR);
        al_destroy_timer(timer);
        al_destroy_display(display);
        al_destroy_event_queue(event_list);
        this->error = true;
    }

    // Inicializacion del addon de figuras primitivas
    if (!al_init_primitives_addon()) {
        al_show_native_message_box(nullptr, "Error!", "Fallo inicialización", "Hubo un error al intentar inicializar el addon de primitivas", nullptr, ALLEGRO_MESSAGEBOX_ERROR);
        al_destroy_timer(timer);
        al_destroy_display(display);
        al_destroy_event_queue(event_list);
        this->error = true;
    }

    // Inicializacion de los addons de fuente y formato True Type Font
    if (!al_init_font_addon()) {
        al_show_native_message_box(nullptr, "Error!", "Fallo inicialización", "Hubo un error al intentar inicializar el addon de fuentes", nullptr, ALLEGRO_MESSAGEBOX_ERROR);
        al_destroy_timer(timer);
        al_destroy_display(display);
        al_destroy_event_queue(event_list);
        this->error = true;
    }

    if (!al_init_ttf_addon()) {
        al_show_native_message_box(nullptr, "Error!", "Fallo inicialización", "Hubo un error al intentar inicializar el addon de fuentes True Type", nullptr, ALLEGRO_MESSAGEBOX_ERROR);
        al_destroy_timer(timer);
        al_destroy_display(display);
        al_destroy_event_queue(event_list);
        this->error = true;
    }

    if (!al_install_audio()) {
        al_show_native_message_box(nullptr, "Error!", "Fallo inicialización", "Hubo un error al intentar inicializar el addon de audio", nullptr, ALLEGRO_MESSAGEBOX_ERROR);
        al_destroy_timer(timer);
        al_destroy_display(display);
        al_destroy_event_queue(event_list);
        this->error = true;
    }

    if (!al_init_acodec_addon()) {
        al_show_native_message_box(nullptr, "Error!", "Fallo inicialización", "Hubo un error al intentar inicializar el codec de audio", nullptr, ALLEGRO_MESSAGEBOX_ERROR);
        al_destroy_timer(timer);
        al_destroy_display(display);
        al_destroy_event_queue(event_list);
        this->error = true;
    }

    if (!al_reserve_samples(2)) {
        al_show_native_message_box(nullptr, "Error!", "Fallo inicialización", "Hubo un error al intentar reservar los samples", nullptr, ALLEGRO_MESSAGEBOX_ERROR);
        al_destroy_timer(timer);
        al_destroy_display(display);
        al_destroy_event_queue(event_list);
        this->error = true;
    }

    // Carga de imagen de titulo y manejo de falla en carga
    titleScreen = al_load_bitmap("Sprites/YAY.jpg");
    if (!titleScreen) {
        al_show_native_message_box(display, "Error!", "Fallo en carga", "Hubo un error al intentar cargar la imagen de titulo", nullptr, ALLEGRO_MESSAGEBOX_ERROR);
        al_destroy_timer(timer);
        al_destroy_display(display);
        al_destroy_event_queue(event_list);
        this->error = true;
    }

    // Carga de imagen de gameover y manejo de fallla de carga
    gameover = al_load_bitmap("Sprites/gameover.png");
    if (!gameover) // si no pudo cargar la imagen
    {
        al_show_native_message_box(display, "Error!", "Fallo en carga", "Hubo un error al intentar cargar la imagen de fin de juego", nullptr, ALLEGRO_MESSAGEBOX_ERROR);
        al_destroy_timer(timer);
        al_destroy_display(display);
        al_destroy_event_queue(event_list);
        this->error = true;
    }

    // Cargar la fuente de los letreros
    billboardFont = al_load_ttf_font("gothic_pixel.ttf", 68, 0);
    if (!billboardFont) {
        al_show_native_message_box(display, "Error!", "Fallo en carga", "Hubo un error al intentar cargar la fuente de los letreros", nullptr, ALLEGRO_MESSAGEBOX_ERROR);
        al_destroy_timer(timer);
        al_destroy_display(display);
        al_destroy_event_queue(event_list);
        this->error = true;
    }

    intro = al_load_sample("Sprites/intro.wav");
    if (!intro) {
        al_show_native_message_box(display, "Error!", "Fallo en carga", "Hubo un error al intentar cargar el audio de intro", nullptr, ALLEGRO_MESSAGEBOX_ERROR);
        al_destroy_timer(timer);
        al_destroy_display(display);
        al_destroy_event_queue(event_list);
        this->error = true;
    }

    spit = al_load_sample("Sprites/spit.wav");
    if (!spit) {
        al_show_native_message_box(display, "Error!", "Fallo en carga", "Hubo un error al intentar cargar el audio de disparo", nullptr, ALLEGRO_MESSAGEBOX_ERROR);
        al_destroy_timer(timer);
        al_destroy_display(display);
        al_destroy_event_queue(event_list);
        this->error = true;
    }

    roaring = al_load_sample("Sprites/roar.wav");
    if (!roaring) {
        al_show_native_message_box(display, "Error!", "Fallo en carga", "Hubo un error al intentar cargar el audio de disparo", nullptr, ALLEGRO_MESSAGEBOX_ERROR);
        al_destroy_timer(timer);
        al_destroy_display(display);
        al_destroy_event_queue(event_list);
        this->error = true;
    }

    // Registro de fuentes de eventos a la cola de eventos
    al_register_event_source(event_list, al_get_display_event_source(display));
    al_register_event_source(event_list, al_get_timer_event_source(timer));
    al_register_event_source(event_list, al_get_keyboard_event_source());

    al_set_window_title(this->getDisplay(), "Sewer breakout!"); // Establece el nombre del juego como titulo de ventana
    bool iniciar = false; // Variable para iniciar el juego
    al_play_sample(this->intro, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, nullptr);
    showTitleScreen(); // Se muestra la pantalla de inicio
    al_draw_text(this->billboardFont, al_map_rgb(77, 113, 9), 510, 307.5, ALLEGRO_ALIGN_CENTER, "Presione Enter");
    al_flip_display(); // Se manda al display

    while (iniciar == false && this->error == false) {
        // Esperamos un evento
        al_wait_for_event(event_list, &ev);

        // Revisa si es un evento de teclado
        if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            switch (ev.keyboard.keycode) {
            case ALLEGRO_KEY_ENTER:
                iniciar = true;
                break;
            case ALLEGRO_KEY_ESCAPE:
                iniciar = true;
                this->stopRunning();
                break;
            default:
                break;
            }
        }
        // Revisa si es el evento es de cierre
        else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            this->stopRunning();
            break;
        }
        al_flush_event_queue(event_list);
    }

    defineMaps();
    createHero();
    try {
        TheManager->initStatsFont();
    } catch (int e) {
        this->error = true;
    }
	this->selectLevel(0);

    if (error) {
        this->stopRunning();
    }
}

Engine::~Engine()
{
    // Destruccion del timer, del display y de la cola de eventos cuando se destruyer el objeto engine
    al_destroy_timer(timer);
    al_destroy_display(display);
    al_destroy_event_queue(event_list);
}

void Engine::createHero()
{
    try {
        this->TheManager->setHero();
    } catch (int e) {
        this->error = true;
    }
}

// TODO: refactorizar gameobjects para obtener dimensiones de los sprites
// TODO: refactorizar sistema de coordenadas para poder cambiar valores a proporciones

void Engine::setFirstLevel()
{
    // Establece el mapa actual
	this->TheManager->setCurrentMap(0);

    // Posiciona al heroe en pantalla
	this->TheManager->mainHeroe->setPosition(20, 200);

    // Posiciona a los enemigos
	this->TheManager->AddGameObject(new Enemy(500, 200, 10, 30, 8, 1, 32, GameObject::LEFT, Enemy::Y, true));
    this->TheManager->AddGameObject(new Enemy(200, 300, 10, 30, 10, 2, 32, GameObject::LEFT, Enemy::X, true));
    this->TheManager->AddGameObject(new Enemy(600, 100, 10, 30, 12, 1, 32, GameObject::RIGHT, Enemy::Y, true));
    this->TheManager->AddGameObject(new Enemy(100, 300, 10, 30, 10, 2, 32, GameObject::LEFT, Enemy::Y, true));
    this->TheManager->AddGameObject(new Enemy(800, 300, 10, 30, 8, 1, 32, GameObject::LEFT, Enemy::X, true));

    // Posiciona los items
    this->TheManager->AddGameObject(new Item(900, 300, 16, 26, Item::POTION));
}

void Engine::setSecondLevel()
{
    // Establece el mapa actual
	this->TheManager->setCurrentMap(1);

    // Posiciona al heroe en pantalla
	this->TheManager->mainHeroe->setPosition(20, 200);

    // Posiciona a los enemigos
    this->TheManager->AddGameObject(new Enemy(500, 100, 10, 30, 8, 1, 32, GameObject::LEFT, Enemy::Y, true));
    this->TheManager->AddGameObject(new Enemy(200, 300, 10, 30, 10, 2, 32, GameObject::LEFT, Enemy::X, true));
    this->TheManager->AddGameObject(new Enemy(600, 100, 10, 30, 12, 1, 32, GameObject::RIGHT, Enemy::Y, true));
    this->TheManager->AddGameObject(new Enemy(100, 300, 10, 30, 10, 2, 32, GameObject::LEFT, Enemy::Y, true));
    this->TheManager->AddGameObject(new Enemy(800, 300, 10, 30, 8, 1, 32, GameObject::LEFT, Enemy::X, true));
}

void Engine::setThirdLevel()
{
    // Establece el mapa actual
	this->TheManager->setCurrentMap(2);

    // Posiciona al heroe en pantalla
	this->TheManager->mainHeroe->setPosition(20, 200);

    // Posiciona al jefe
    this->TheManager->AddGameObject(new Boss(850, 200, 45, 50, 8, 1, 32, GameObject::LEFT, this->roaring));
}

void Engine::selectLevel(int level)
{
	if (level != this->currentLevel || this->TheManager->mainHeroe->restart)
	{
		this->TheManager->clearGameObjectList();
		this->TheManager->informLevel(level);
		this->currentLevel = level;
		switch (level) {
		case 0:
			this->setFirstLevel();
			break;
		case 1:
			this->setSecondLevel();
			break;
		case 2:
			this->setThirdLevel();
			break;
		default:
			break;
		}
		this->TheManager->mainHeroe->updateMap(this->TheManager->getCurrentMap());
	}
}

void Engine::defineMaps()
{
    try {
        this->TheManager->AddMap(new Mapa(this->getDisplay(), "Sprites/ESCENARIO1.png", "Sprites/ESCENARIO1_COLLISION.png"));
        this->TheManager->AddMap(new Mapa(this->getDisplay(), "Sprites/ESCENARIO2.png", "Sprites/ESCENARIO2_COLLISION.png"));
        this->TheManager->AddMap(new Mapa(this->getDisplay(), "Sprites/ESCENARIO3.png", "Sprites/ESCENARIO3_COLLISION.png"));
    } catch (std::string filename) {
        al_show_native_message_box(this->getDisplay(), "Error!", "Error al intentar cargar la imagen:", filename.c_str(), nullptr, ALLEGRO_MESSAGEBOX_ERROR);
        this->error = true;
    }
}

void Engine::keyPressed(int key)
{
    if (!TheManager->GameOver) {

        switch (key) {
        case ALLEGRO_KEY_RIGHT:
        case ALLEGRO_KEY_D:
            this->TheManager->mainHeroe->move(0);
            break;
        case ALLEGRO_KEY_LEFT:
        case ALLEGRO_KEY_A:
            this->TheManager->mainHeroe->move(1);
            break;
        case ALLEGRO_KEY_DOWN:
        case ALLEGRO_KEY_S:
            this->TheManager->mainHeroe->move(2);
            break;
        case ALLEGRO_KEY_UP:
        case ALLEGRO_KEY_W:
            this->TheManager->mainHeroe->move(3);
            break;
        default:
            break;
        }
    } else {
        this->stopRunning();
    }
}

void Engine::shoot()
{
    al_play_sample(this->spit, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, nullptr);
    this->TheManager->AddGameObject(new Missile(this->TheManager->mainHeroe->getX(),
        this->TheManager->mainHeroe->getY(),
        23.0, 25.0, 10, this->TheManager->mainHeroe->getLastDirection()));
}

void Engine::keyReleased(int key)
{
    switch (key) {
    case ALLEGRO_KEY_RIGHT:
    case ALLEGRO_KEY_D:
        this->TheManager->mainHeroe->stop(0);
        break;
    case ALLEGRO_KEY_LEFT:
    case ALLEGRO_KEY_A:
        this->TheManager->mainHeroe->stop(1);
        break;
    case ALLEGRO_KEY_DOWN:
    case ALLEGRO_KEY_S:
        this->TheManager->mainHeroe->stop(2);
        break;
    case ALLEGRO_KEY_UP:
    case ALLEGRO_KEY_W:
        this->TheManager->mainHeroe->stop(3);
        break;
    case ALLEGRO_KEY_SPACE:
        this->shoot();
        break;
	case ALLEGRO_KEY_P:
		this->pause = !this->pause;
		break;
    default:
        break;
    }
}

void Engine::handleEvents()
{

    switch (this->ev.type) {
    case ALLEGRO_EVENT_KEY_DOWN:
        keyPressed(ev.keyboard.keycode);
        break;
    case ALLEGRO_EVENT_KEY_UP:
        keyReleased(ev.keyboard.keycode);
        break;
    case ALLEGRO_EVENT_TIMER:
        redraw = true;
        break;
    case ALLEGRO_EVENT_DISPLAY_CLOSE:
        this->stopRunning();
    default:
        break;
    }
}

void Engine::showPause()
{
	al_draw_text(this->billboardFont, al_map_rgb(255, 255, 255), 510, 100, ALLEGRO_ALIGN_CENTER, "Pausa");
	al_flip_display();
}

void Engine::draw()
{
	if (!pause)
	{
		if (!TheManager->GameOver) {
			if (this->redraw) {
				al_clear_to_color(al_map_rgb(0, 0, 0));
				this->TheManager->getCurrentMap()->drawMap(this->TheManager->mainHeroe->getX(), this->TheManager->mainHeroe->getY());
				this->TheManager->mainHeroe->draw();
				this->TheManager->DrawGameObjects();
				this->TheManager->DrawStats();
				this->redraw = false;
				al_flip_display();
			}
		}
		else {
			al_clear_to_color(al_map_rgb(0, 0, 0));
			this->showgameover();
			al_flip_display();
		}
	}
	else {
		this->showPause();
	}
}

void Engine::update()
{
	if (!this->pause)
	{
		if (this->TheManager->mainHeroe->isDead()) {
			TheManager->GameOver = true;
			return;
		}

		if (this->TheManager->mainHeroe->restart)
		{
			this->selectLevel(this->currentLevel);
			this->TheManager->mainHeroe->restart = false;
			return;
		}

		if (this->TheManager->getEnemiesCount() == 0 && this->TheManager->mainHeroe->nextLevel)
		{
			this->TheManager->mainHeroe->nextLevel = false;
			if (this->currentLevel != 2)
			{
				this->selectLevel(this->currentLevel + 1);
			}
		}

		this->TheManager->CheckCollissions();
		this->TheManager->mainHeroe->update();
		this->TheManager->UpdateGameObjects();
	}

}

// Regresa el estado del juego
bool Engine::isRunning()
{
    return running;
}

// Detiene el juego
void Engine::stopRunning()
{
    running = false;
}

// Muestra la pantalla de bienvenida
void Engine::showTitleScreen()
{
    al_draw_bitmap(titleScreen, 10, 10, 0);
}

// Muestra la pantalla de gameover
void Engine::showgameover()
{
    al_draw_bitmap(gameover, 200, 50, 0);
}

// Muestra los creditos finales
void Engine::showCredits()
{
    ALLEGRO_TRANSFORM transform;
    al_identity_transform(&transform);
    al_use_transform(&transform);
    al_clear_to_color(al_map_rgb(0, 0, 0));
    this->showTitleScreen();
    al_draw_text(this->billboardFont, al_map_rgb(255, 255, 255), 510, 180, ALLEGRO_ALIGN_CENTER, "Creditos");
    al_flip_display();
    al_rest(2);

    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_draw_text(this->billboardFont, al_map_rgb(255, 255, 255), 510, 100, ALLEGRO_ALIGN_CENTER, "Arte");
    al_draw_text(this->billboardFont, al_map_rgb(255, 255, 255), 510, 180, ALLEGRO_ALIGN_CENTER, "Andrea Vanessa Delgado Huerta");
    al_draw_text(this->billboardFont, al_map_rgb(255, 255, 255), 510, 260, ALLEGRO_ALIGN_CENTER, "Alejandra Morales Lopez");
    al_draw_text(this->billboardFont, al_map_rgb(255, 255, 255), 510, 340, ALLEGRO_ALIGN_CENTER, "Karla Mercedes Hernandez Pascacio");
    al_flip_display();
    al_rest(2);

    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_draw_text(this->billboardFont, al_map_rgb(255, 255, 255), 510, 100, ALLEGRO_ALIGN_CENTER, "Codigo");
    al_draw_text(this->billboardFont, al_map_rgb(255, 255, 255), 510, 260, ALLEGRO_ALIGN_CENTER, "Juan Manuel Morales Garcia");
    al_flip_display();
    al_rest(2);
}

// Devuelve el display actual
ALLEGRO_DISPLAY* Engine::getDisplay()
{
    return display;
}
