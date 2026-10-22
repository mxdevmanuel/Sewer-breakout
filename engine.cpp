#include "engine.h"
#include "boss.h"
#include "enemy.h"
#include "hero.h"
#include "mapa.h"
#include "missile.h"
#include "item.h"
#include "level.h"
#include <filesystem>

Engine::Engine(int width, int height)
{
    // Inicializar variables
    this->running = true;
    this->fps = 60;
    this->redraw = false;
    this->fullScreen = false;
    this->error = false;
	this->currentLevel = -1;

    // Inicializar allegro, sus addons y los recursos; ante el primer fallo se detiene el juego
    if (!al_init()) {
        fail("Fallo inicialización", "Hubo un error al intentar inicializar Allegro");
        return;
    }
    al_init_native_dialog_addon();

    timer = al_create_timer(1.0 / fps);
    if (!timer) {
        fail("Fallo inicialización", "Hubo un error al intentar inicializar el timer");
        return;
    }

    display = al_create_display(width, height);
    if (!display) {
        fail("Fallo inicialización", "Hubo un error al intentar inicializar el display");
        return;
    }

    event_list = al_create_event_queue();
    if (!event_list) {
        fail("Fallo inicialización", "Hubo un error al intentar inicializar la cola de eventos");
        return;
    }

    if (!al_install_keyboard()) {
        fail("Fallo inicialización", "Hubo un error al intentar inicializar el teclado");
        return;
    }
    if (!al_init_image_addon()) {
        fail("Fallo inicialización", "Hubo un error al intentar inicializar el addon de imagenes");
        return;
    }
    if (!al_init_primitives_addon()) {
        fail("Fallo inicialización", "Hubo un error al intentar inicializar el addon de primitivas");
        return;
    }
    if (!al_init_font_addon()) {
        fail("Fallo inicialización", "Hubo un error al intentar inicializar el addon de fuentes");
        return;
    }
    if (!al_init_ttf_addon()) {
        fail("Fallo inicialización", "Hubo un error al intentar inicializar el addon de fuentes True Type");
        return;
    }
    if (!al_install_audio()) {
        fail("Fallo inicialización", "Hubo un error al intentar inicializar el addon de audio");
        return;
    }
    if (!al_init_acodec_addon()) {
        fail("Fallo inicialización", "Hubo un error al intentar inicializar el codec de audio");
        return;
    }
    if (!al_reserve_samples(2)) {
        fail("Fallo inicialización", "Hubo un error al intentar reservar los samples");
        return;
    }

    // Carga de imagenes, fuente y audios
    titleScreen = al_load_bitmap("Sprites/YAY.jpg");
    if (!titleScreen) {
        fail("Fallo en carga", "Hubo un error al intentar cargar la imagen de titulo");
        return;
    }
    gameover = al_load_bitmap("Sprites/gameover.png");
    if (!gameover) {
        fail("Fallo en carga", "Hubo un error al intentar cargar la imagen de fin de juego");
        return;
    }
    billboardFont = al_load_ttf_font("gothic_pixel.ttf", 68, 0);
    if (!billboardFont) {
        fail("Fallo en carga", "Hubo un error al intentar cargar la fuente de los letreros");
        return;
    }
    intro = al_load_sample("Sprites/intro.wav");
    if (!intro) {
        fail("Fallo en carga", "Hubo un error al intentar cargar el audio de intro");
        return;
    }
    spit = al_load_sample("Sprites/spit.wav");
    if (!spit) {
        fail("Fallo en carga", "Hubo un error al intentar cargar el audio de disparo");
        return;
    }
    roaring = al_load_sample("Sprites/roar.wav");
    if (!roaring) {
        fail("Fallo en carga", "Hubo un error al intentar cargar el audio del rugido");
        return;
    }

    // Registro de fuentes de eventos a la cola de eventos
    al_register_event_source(event_list, al_get_display_event_source(display));
    al_register_event_source(event_list, al_get_timer_event_source(timer));
    al_register_event_source(event_list, al_get_keyboard_event_source());

    al_set_window_title(this->getDisplay(), "Sewer breakout!"); // Establece el nombre del juego como titulo de ventana

    loadLevels();
    if (error) {
        return;
    }
    createHero();
    try {
        TheManager->initStatsFont();
    } catch (int e) {
        this->error = true;
    }

    if (error) {
        this->stopRunning();
        return;
    }

    // El juego arranca en la pantalla de titulo; el nivel se carga al presionar Enter
    al_play_sample(this->intro, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, nullptr);
    this->state = GameState::Title;
    this->redraw = true;
}

Engine::~Engine()
{
    // Destruccion de los recursos creados, que pueden faltar si la inicializacion fallo
    if (roaring) al_destroy_sample(roaring);
    if (spit) al_destroy_sample(spit);
    if (intro) al_destroy_sample(intro);
    if (billboardFont) al_destroy_font(billboardFont);
    if (gameover) al_destroy_bitmap(gameover);
    if (titleScreen) al_destroy_bitmap(titleScreen);
    if (event_list) al_destroy_event_queue(event_list);
    if (display) al_destroy_display(display);
    if (timer) al_destroy_timer(timer);
}

// Muestra el error, marca la falla y detiene el juego
void Engine::fail(const char *heading, const char *message)
{
    al_show_native_message_box(display, "Error!", heading, message, nullptr, ALLEGRO_MESSAGEBOX_ERROR);
    this->error = true;
    this->stopRunning();
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

void Engine::selectLevel(int level)
{
	if (level != this->currentLevel || this->TheManager->mainHeroe->restart)
	{
		this->TheManager->clearGameObjectList();
		this->TheManager->informLevel(level);
		this->currentLevel = level;

		// Establece el mapa, posiciona al heroe y coloca los objetos del nivel
		const LevelData &data = this->levels[level];
		this->TheManager->setCurrentMap(level);
		this->TheManager->mainHeroe->setPosition(data.heroX, data.heroY);
		for (const Spawn &spawn : data.spawns) {
			switch (spawn.kind) {
			case Spawn::ENEMY:
				this->TheManager->AddGameObject(new Enemy(spawn.x, spawn.y, spawn.width, spawn.height, spawn.hits, spawn.speed, spawn.distance, spawn.direction, spawn.axis, true));
				break;
			case Spawn::ITEM:
				this->TheManager->AddGameObject(new Item(spawn.x, spawn.y, spawn.width, spawn.height, spawn.itemType));
				break;
			case Spawn::BOSS:
				this->TheManager->AddGameObject(new Boss(spawn.x, spawn.y, spawn.width, spawn.height, spawn.hits, spawn.speed, spawn.distance, spawn.direction, this->roaring));
				break;
			}
		}
		this->TheManager->mainHeroe->updateMap(this->TheManager->getCurrentMap());
	}
}

// Carga levels/level1.txt, level2.txt, ... hasta que falte un archivo, con un mapa por nivel
void Engine::loadLevels()
{
    try {
        for (int i = 1;; ++i) {
            std::string path = "levels/level" + std::to_string(i) + ".txt";
            if (!std::filesystem::exists(path)) {
                break;
            }
            this->levels.push_back(loadLevel(path));
            const LevelData &data = this->levels.back();
            this->TheManager->AddMap(new Mapa(this->getDisplay(), data.mapFile, data.collisionFile));
        }
    } catch (const std::string &filename) {
        fail("Error al intentar cargar la imagen:", filename.c_str());
        return;
    } catch (const std::runtime_error &e) {
        fail("Error en archivo de nivel:", e.what());
        return;
    }

    if (this->levels.empty()) {
        fail("Fallo en carga", "No se encontraron niveles en levels/");
    }
}

void Engine::keyPressed(int key)
{
    switch (this->state) {
    case GameState::Title:
        if (key == ALLEGRO_KEY_ENTER) {
            this->selectLevel(0);
            this->state = GameState::Playing;
        } else if (key == ALLEGRO_KEY_ESCAPE) {
            this->startCredits();
        }
        break;
    case GameState::Playing:
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
        break;
    case GameState::GameOver:
        // Cualquier tecla en la pantalla de fin de juego pasa a los creditos
        this->startCredits();
        break;
    default:
        break;
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
    // Las teclas soltadas durante la pausa tambien detienen al heroe
    if (this->state != GameState::Playing && this->state != GameState::Paused) {
        return;
    }

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
        if (this->state == GameState::Playing) {
            this->shoot();
        }
        break;
    case ALLEGRO_KEY_P:
        this->state = this->state == GameState::Playing ? GameState::Paused : GameState::Playing;
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
        break;
    default:
        break;
    }
}

void Engine::showPause()
{
    al_draw_text(this->billboardFont, al_map_rgb(255, 255, 255), 510, 100, ALLEGRO_ALIGN_CENTER, "Pausa");
}

// Dibuja el mapa, el heroe, los objetos y las estadisticas
void Engine::drawScene()
{
    this->TheManager->getCurrentMap()->drawMap(this->TheManager->mainHeroe->getX(), this->TheManager->mainHeroe->getY());
    this->TheManager->mainHeroe->draw();
    this->TheManager->DrawGameObjects();
    this->TheManager->DrawStats();
}

void Engine::draw()
{
    // Solo se dibuja en un tick del timer y cuando no hay eventos pendientes, para no atrasarse
    if (!this->redraw || !al_is_event_queue_empty(this->event_list)) {
        return;
    }
    this->redraw = false;

    al_clear_to_color(al_map_rgb(0, 0, 0));
    switch (this->state) {
    case GameState::Title:
        this->showTitleScreen();
        al_draw_text(this->billboardFont, al_map_rgb(77, 113, 9), 510, 307.5, ALLEGRO_ALIGN_CENTER, "Presione Enter");
        break;
    case GameState::Playing:
        this->drawScene();
        break;
    case GameState::Paused:
        this->drawScene();
        this->showPause();
        break;
    case GameState::GameOver:
        this->showgameover();
        break;
    case GameState::Credits:
        this->drawCreditsPage(this->creditsTicks / creditsPageTicks);
        break;
    }
    al_flip_display();
}

void Engine::update()
{
    // La logica avanza solo con los ticks del timer, no con cada evento de teclado
    if (this->ev.type != ALLEGRO_EVENT_TIMER) {
        return;
    }

    switch (this->state) {
    case GameState::Playing:
        this->updatePlaying();
        break;
    case GameState::Credits:
        if (++this->creditsTicks >= creditsPages * creditsPageTicks) {
            this->stopRunning();
        }
        break;
    default:
        break;
    }
}

void Engine::updatePlaying()
{
    if (this->TheManager->mainHeroe->isDead()) {
        TheManager->GameOver = true;
        this->state = GameState::GameOver;
        return;
    }

    if (this->TheManager->mainHeroe->restart) {
        this->selectLevel(this->currentLevel);
        this->TheManager->mainHeroe->restart = false;
        return;
    }

    if (this->TheManager->getEnemiesCount() == 0 && this->TheManager->mainHeroe->nextLevel) {
        this->TheManager->mainHeroe->nextLevel = false;
        if (this->currentLevel + 1 < static_cast<int>(this->levels.size())) {
            this->selectLevel(this->currentLevel + 1);
        }
    }

    this->TheManager->CheckCollissions();
    this->TheManager->mainHeroe->update();
    this->TheManager->UpdateGameObjects();
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

// Pasa a los creditos finales, que avanzan con los ticks del timer
void Engine::startCredits()
{
    this->creditsTicks = 0;
    this->state = GameState::Credits;
}

// Dibuja la pagina `page` de los creditos finales
void Engine::drawCreditsPage(int page)
{
    ALLEGRO_TRANSFORM transform;
    al_identity_transform(&transform);
    al_use_transform(&transform);

    switch (page) {
    case 0:
        this->showTitleScreen();
        al_draw_text(this->billboardFont, al_map_rgb(255, 255, 255), 510, 180, ALLEGRO_ALIGN_CENTER, "Creditos");
        break;
    case 1:
        al_draw_text(this->billboardFont, al_map_rgb(255, 255, 255), 510, 100, ALLEGRO_ALIGN_CENTER, "Arte");
        al_draw_text(this->billboardFont, al_map_rgb(255, 255, 255), 510, 180, ALLEGRO_ALIGN_CENTER, "Andrea Vanessa Delgado Huerta");
        al_draw_text(this->billboardFont, al_map_rgb(255, 255, 255), 510, 260, ALLEGRO_ALIGN_CENTER, "Alejandra Morales Lopez");
        al_draw_text(this->billboardFont, al_map_rgb(255, 255, 255), 510, 340, ALLEGRO_ALIGN_CENTER, "Karla Mercedes Hernandez Pascacio");
        break;
    default:
        al_draw_text(this->billboardFont, al_map_rgb(255, 255, 255), 510, 100, ALLEGRO_ALIGN_CENTER, "Codigo");
        al_draw_text(this->billboardFont, al_map_rgb(255, 255, 255), 510, 260, ALLEGRO_ALIGN_CENTER, "Juan Manuel Morales Garcia");
        break;
    }
}

// Devuelve el display actual
ALLEGRO_DISPLAY* Engine::getDisplay()
{
    return display;
}
