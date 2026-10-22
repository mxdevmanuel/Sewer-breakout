#ifndef ENEMY_H
#define ENEMY_H
#include "gameobject.h"

class Enemy: public GameObject
{
private:
    int hits, speed, distance, direction, hitCount, axis, originX, originY, acceleration;
    bool verticalChange;
    bool atOrigin = false; // Si en el cuadro anterior estaba en su origen

public:
    Enemy(float x, float y, float width, float height, float hits, int speed, int distance, int direction, int axis, bool moving);

    enum positionNames {EASY, ATTACK, FURIOUS};
    enum axis { X, Y };
    void draw();
    void update();
    void collision(GameObject *cw);
    void setSprites();
    bool toDestroy();
};

#endif // ENEMY_H
