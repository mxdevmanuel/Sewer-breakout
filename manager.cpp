#include "manager.h"
#include "gameobject.h"
#include "hero.h"
#include "enemymissile.h"
#include <iostream>

Manager* Manager::instance = nullptr;

Manager::Manager()
{
    GameOver = false;
    bossSaved = false;
    bossref = nullptr;
    setCurrentMap(0);
}

Manager* Manager::Instance()
{
    if (instance == nullptr) {
        instance = new Manager();
    }

    return instance;
}

void Manager::initStatsFont()
{
    this->statsFont = al_load_ttf_font("Sprites/stats.ttf", 32, 0);
    if (!this->statsFont) {
        al_show_native_message_box(NULL, "Error!", "Fallo inicialización", "Hubo un error al intentar inicializar la fuente", NULL, ALLEGRO_MESSAGEBOX_ERROR);
        throw 20;
    }
}

void Manager::clearGameObjectList()
{
	for (GameObject *object : this->gameObjectList) {
		delete object;
	}
	this->gameObjectList.clear();

	// La referencia al jefe apuntaria a un objeto borrado
	this->bossref = nullptr;
	this->bossSaved = false;
}

void Manager::setHero()
{
    mainHeroe = new Hero(20, 200, 24, 33, 3, 5);
    mainHeroe->updateMap(getCurrentMap());
}

void Manager::setCurrentMap(int index)
{
    this->currentMap = index;
}

Mapa* Manager::getCurrentMap()
{
    return this->mapList[this->currentMap];
}

void Manager::AddGameObject(GameObject *newObject)
{
    gameObjectList.push_back(newObject);
}

void Manager::AddMap(Mapa *newMap)
{
    this->mapList.push_back(newMap);
}

void Manager::DrawStats()
{
    al_identity_transform(&transform);
    al_use_transform(&transform);
    al_draw_text(statsFont, al_map_rgb(255, 255, 255), 10, 5, ALLEGRO_ALIGN_LEFT, "Vidas:");
    al_draw_text(statsFont, al_map_rgb(255, 255, 255), 140, 5, ALLEGRO_ALIGN_LEFT, std::to_string(this->mainHeroe->getLives()).c_str());
    al_draw_text(statsFont, al_map_rgb(255, 255, 255), 280, 5, ALLEGRO_ALIGN_LEFT, "Puntos:");
    al_draw_text(statsFont, al_map_rgb(255, 255, 255), 420, 5, ALLEGRO_ALIGN_LEFT, std::to_string(this->mainHeroe->getPoints()).c_str());
	al_draw_text(statsFont, al_map_rgb(255, 255, 255), 550, 5, ALLEGRO_ALIGN_LEFT, "Nivel:");
	al_draw_text(statsFont, al_map_rgb(255, 255, 255), 700, 5, ALLEGRO_ALIGN_LEFT, std::to_string(this->currentLevel + 1).c_str());
}

void Manager::DrawGameObjects()
{
    for(std::vector<GameObject*>::iterator it = this->gameObjectList.begin(); it != this->gameObjectList.end(); ++it)
    {
        (*it)->draw();
    }
}

void Manager::UpdateGameObjects()
{
	this->enemies = 0;
    std::cout << "Game object list count: " << gameObjectList.size() << std::endl;
    for(std::vector<GameObject*>::iterator it = this->gameObjectList.begin(); it != this->gameObjectList.end();)
    {
        (*it)->update();
		if ((*it)->getType() == GameObject::enemy)
		{
			this->enemies++;
		}

        if ((*it)->getType() == GameObject::boss)
        {
            if (!this->bossSaved) {
                this->bossref = static_cast<Boss*>(*it);
                this->bossSaved = true;
            }
            this->enemies++;

            if (this->bossref->positionChanged()) {
                //AddGameObject(new EnemyMissile(this->bossref->getX(), this->bossref->getY(), 27, 14, 3, GameObject::LEFT));
            }
        }

        if ((*it)->toDestroy()) {
            delete *it;
			this->mainHeroe->setPoints(this->mainHeroe->getPoints() + 100);
            it = this->gameObjectList.erase(it);
        } else {
            ++it;
        }
    }
}

int Manager::getEnemiesCount()
{
	return this->enemies;
}

void Manager::boundingBox(GameObject *receiver, GameObject *collisioner)
{
        if ((receiver->getX() > collisioner->getX() + collisioner->getWidth() - 1) || // is b1 on the right side of b2?
            (receiver->getY() > collisioner->getY() + collisioner->getHeight() - 1) || // is b1 under b2?
            (collisioner->getX() > receiver->getX() + receiver->getWidth() - 1) || // is b2 on the right side of b1?
            (collisioner->getY() > receiver->getY() + receiver->getHeight() - 1))   // is b2 under b1?
        {
            return;
        }
        receiver->collision(collisioner);
}

void Manager::CheckCollissions()
{
    for (std::vector<GameObject*>::iterator ot = this->gameObjectList.begin(); ot != this->gameObjectList.end(); ++ot) {
        if ((*ot)->getType() == GameObject::item) {
            std::cout<<"Item collision"<<std::endl;
            boundingBox(*(ot), this->mainHeroe); // Checa colisiones del item con el player
        }
        boundingBox(this->mainHeroe, *(ot)); // Checa colisiones del objeto actual con el player
        for (std::vector<GameObject*>::iterator it = this->gameObjectList.begin(); it != this->gameObjectList.end(); ++it) {
            if (*it != *ot) {
                boundingBox(*(it), *(ot)); // Checa colisiones entre objetos
            }
        }
    }


}

void Manager::informLevel(int level)
{
	this->currentLevel = level;
}

Manager::~Manager()
{

}
