#ifndef ENEMY_H
#define ENEMY_H
#include "gameobject.h"

class Enemy: public GameObject
{
private:
    int hits, speed, distance, direction, hitCount, axis, originX, originY, acceleration;
    bool verticalChange;
    bool atOrigin = false; // Si en el cuadro anterior estaba en su origen
    int furyPattern = 0; // Patron de movimiento enfurecido (CROSS o DIAMOND)
    int diamondStep = 0; // Vertice del rombo hacia el que se mueve

    void patrol(); // Ida y vuelta sobre el eje actual
    void chooseFuryPattern(); // Elige al azar el patron enfurecido al pasar por el origen
    void moveDiamond(); // Avanza un paso sobre el rombo alrededor del origen

public:
    Enemy(float x, float y, float width, float height, float hits, int speed, int distance, int direction, int axis, bool moving);

    enum positionNames {EASY, ATTACK, FURIOUS};
    enum axis { X, Y };
    enum furyPatterns { CROSS, DIAMOND };
    void draw();
    void update();
    void collision(GameObject *cw);
    void setSprites();
    bool toDestroy();
};

#endif // ENEMY_H
