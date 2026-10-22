#include "enemy.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>

Enemy::Enemy(float x, float y, float width, float height, float hits, int speed, int distance, int direction, int axis, bool moving)
    : GameObject(x, y, width, height)
{
    this->hits = static_cast<int>(hits);
    this->hitCount = 0;
    this->direction = direction;
    this->speed = speed;
    this->acceleration = speed;
    this->moving = moving;
    this->axis = axis;
    this->distance = distance;
    this->type = GameObject::enemy;
    this->originX = getX();
    this->originY = getY();
    this->verticalChange = true;
    this->loadSpriteSheet("Sprites/cucahabla.png");
    setSprites();
	this->currentPosition = &positions[Enemy::EASY];
}

void Enemy::setSprites()
{
    this->positions.insert(std::pair<int, Sprite>(Enemy::EASY, { 0, 31, 29, 35 }));
    this->positions.insert(std::pair<int, Sprite>(Enemy::ATTACK, { 36, 32, 27, 35 }));
    this->positions.insert(std::pair<int, Sprite>(Enemy::FURIOUS, { 70, 31, 26, 37 }));
}

void Enemy::update()
{
    if (this->moving) {

        // Enfurecido, cada vez que pasa por su origen elige al azar entre cruz y rombo
        bool furious = this->currentPosition == &positions[Enemy::FURIOUS];
        if (furious && this->furyPattern == Enemy::DIAMOND) {
            this->moveDiamond();
        } else if (furious) {
            bool nearOrigin = std::abs(this->getX() - static_cast<float>(this->originX)) < static_cast<float>(this->speed)
                && std::abs(this->getY() - static_cast<float>(this->originY)) < static_cast<float>(this->speed);
            if (nearOrigin && !this->atOrigin) {
                this->chooseFuryPattern();
            }
            this->atOrigin = nearOrigin; // Solo elige una vez por pasada, aunque siga cerca del origen
        }

        // Patrulla de ida y vuelta sobre su eje, que tambien forma la cruz al estar enfurecido
        if (!furious || this->furyPattern == Enemy::CROSS) {
            this->patrol();
        }

    }

    if (hitCount > ((int)hits / 4)*3) {
        this->currentPosition = &positions[Enemy::FURIOUS];
        this->speed = this->acceleration * 2;
    } else if(hitCount > ((int)hits / 2)) {
        this->currentPosition = &positions[Enemy::ATTACK];
        this->speed = static_cast<int>(this->acceleration * 1.5);
    } else {
        this->currentPosition = &positions[Enemy::EASY];
    }
}

// Avanza un paso de ida y vuelta sobre el eje actual, a lo mas `distance` del origen
void Enemy::patrol()
{
    switch (this->axis) {
    case Enemy::X:
        if (this->direction == GameObject::RIGHT) {
            if (this->getX() >= this->originX + this->distance) {
                this->direction = GameObject::LEFT;
            } else {
                this->x += speed;
            }
        } else {
            if (this->getX() <= this->originX - this->distance) {
                this->direction = GameObject::RIGHT;
            } else {
                this->x -= speed;
            }
        }
        break;
    case Enemy::Y:
        if (verticalChange) {
            if (this->getY() >= this->originY + this->distance) {
                verticalChange = false;
            } else {
                this->y += speed;
            }
        } else {
            if (this->getY() <= this->originY - this->distance) {
                verticalChange = true;
            } else {
                this->y -= speed;
            }
        }
    default:
        break;
    }
}

void Enemy::chooseFuryPattern()
{
    if (rand() % 2 == 0) {
        this->furyPattern = Enemy::DIAMOND;
        this->diamondStep = 0;
    } else {
        this->axis = this->axis == Enemy::X ? Enemy::Y : Enemy::X;
    }
}

// Recorre los cuatro vertices del rombo alrededor del origen y regresa al centro,
// donde vuelve a elegir patron
void Enemy::moveDiamond()
{
    static const int vertices[5][2] = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}, {0, 0}};
    auto targetX = static_cast<float>(this->originX + vertices[this->diamondStep][0] * this->distance);
    auto targetY = static_cast<float>(this->originY + vertices[this->diamondStep][1] * this->distance);
    auto step = static_cast<float>(this->speed);

    float dx = std::clamp(targetX - this->x, -step, step);
    float dy = std::clamp(targetY - this->y, -step, step);
    this->x += dx;
    this->y += dy;
    if (dx != 0) {
        this->direction = dx > 0 ? GameObject::RIGHT : GameObject::LEFT;
    }

    if (this->x == targetX && this->y == targetY) {
        this->diamondStep++;
        if (this->diamondStep == 5) {
            // De vuelta en el origen: el siguiente cuadro elige patron otra vez
            this->furyPattern = Enemy::CROSS;
            this->diamondStep = 0;
            this->atOrigin = false;
        }
    }
}

void Enemy::draw()
{
    al_draw_bitmap_region(this->spriteSheet,
                          this->currentPosition->sx,
                          this->currentPosition->sy,
                          this->currentPosition->sw,
                          this->currentPosition->sh,
                          this->x,
                          this->y,
                          this->direction == GameObject::LEFT ? ALLEGRO_FLIP_HORIZONTAL : 0);
}

void Enemy::collision(GameObject *cw)
{
    if (cw->getType() == GameObject::missile) {
        this->hitCount++;
    }
}

bool Enemy::toDestroy()
{
    return (this->hitCount >= this->hits);
}
