#ifndef HERO_H
#define HERO_H
#include <allegro5/allegro_audio.h>
#include <map>
#include "gameobject.h"
#include "mapa.h"

class Hero : public GameObject
{
  private:
    int lives;
    int speed;
    int lastDirection;
    int lastPosition;
    int points;
    bool directions[4] = {false, false, false, false};
    ALLEGRO_BITMAP* collisionMap;
    ALLEGRO_COLOR currentColors[2]{};
    Mapa* current;
    bool isBoundary();
    std::map<int, int> itemList;

  public:
    Hero(float x, float y, float width, float height, int lives, int speed);
    enum positionNames
    {
        STANDING,
        WALKING
    };
    void move(int direction);
    void stop(int direction);
    void setSprites();
    void updateMap(Mapa* current);
    void draw();
    void update();
    void collision(GameObject* cw);
    void die();
    bool toDestroy()
    {
        return false;
    }
    bool isDead();
    bool mustStartAgain();
    bool restart = false;
    bool nextLevel;
    int getLastDirection();
    int getPoints();
    void setPoints(int points);
    int getLives();
    void setItemList();
};

#endif // HERO_H
