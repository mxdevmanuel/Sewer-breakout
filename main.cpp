#include <allegro5/allegro.h>
#include "engine.h"

int main()
{

    // Random
    srand(time(nullptr));

    // Creamos el engine del juego
    auto *engine = new Engine(1020,450);
    // Iniciamos el timer del juego si la inicializacion fue correcta
    if (engine->isRunning()) {
        al_start_timer(engine->timer);
    }
    // Iniciamos el bucle principal del juego
    while(engine->isRunning())
    {
        // Aguardamos a que se dispare algun evento
        al_wait_for_event(engine->event_list, &engine->ev);

        // Llamamos a la funcion del motor que maneja los eventos
        engine->handleEvents();
        engine->update();
        engine->draw();

    }

    // Destruimos el motor del juego y regresamos el control al sistema operativo
    delete engine;
    return 0;
}
