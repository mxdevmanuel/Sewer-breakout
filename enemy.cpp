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

        // Enfurecido, elige al azar entre cruz y rombo al pasar por su origen y en cada vuelta
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
            bool turned = this->patrol();
            if (furious && turned) {
                this->furyTurn();
            }
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

// Avanza un paso de ida y vuelta sobre el eje actual, a lo mas `distance` del origen;
// devuelve true si en este paso dio la vuelta
bool Enemy::patrol()
{
    bool turned = false;
    switch (this->axis) {
    case Enemy::X:
        if (this->direction == GameObject::RIGHT) {
            if (this->getX() >= this->originX + this->distance) {
                this->direction = GameObject::LEFT;
                turned = true;
            } else {
                this->x += speed;
            }
        } else {
            if (this->getX() <= this->originX - this->distance) {
                this->direction = GameObject::RIGHT;
                turned = true;
            } else {
                this->x -= speed;
            }
        }
        break;
    case Enemy::Y:
        if (verticalChange) {
            if (this->getY() >= this->originY + this->distance) {
                verticalChange = false;
                turned = true;
            } else {
                this->y += speed;
            }
        } else {
            if (this->getY() <= this->originY - this->distance) {
                verticalChange = true;
                turned = true;
            } else {
                this->y -= speed;
            }
        }
    default:
        break;
    }
    return turned;
}

// Al pasar por el origen: cambia de eje o empieza un rombo
void Enemy::chooseFuryPattern()
{
    if (rand() % 2 == 0) {
        this->startDiamond();
    } else {
        this->axis = this->axis == Enemy::X ? Enemy::Y : Enemy::X;
    }
}

// Al dar la vuelta en la cruz: sigue igual, cambia de eje o empieza un rombo
void Enemy::furyTurn()
{
    switch (rand() % 3) {
    case 1:
        this->axis = this->axis == Enemy::X ? Enemy::Y : Enemy::X;
        break;
    case 2:
        this->startDiamond();
        break;
    default:
        break;
    }
}

// Empieza un rombo con sentido, vertice inicial y tamaño al azar (entre 0.5 y 1.5 veces `distance`)
void Enemy::startDiamond()
{
    this->furyPattern = Enemy::DIAMOND;
    this->diamondDir = rand() % 2 == 0 ? 1 : -1;
    this->diamondCorner = rand() % 4;
    this->diamondSize = this->distance * (50 + rand() % 101) / 100;
}

// Avanza un paso hacia el vertice actual del rombo alrededor del origen; en cada vertice
// sigue (50%), se regresa (25%) o vuelve a la cruz rumbo al origen (25%)
void Enemy::moveDiamond()
{
    static const int vertices[4][2] = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};
    auto targetX = static_cast<float>(this->originX + vertices[this->diamondCorner][0] * this->diamondSize);
    auto targetY = static_cast<float>(this->originY + vertices[this->diamondCorner][1] * this->diamondSize);
    auto step = static_cast<float>(this->speed);

    float dx = std::clamp(targetX - this->x, -step, step);
    float dy = std::clamp(targetY - this->y, -step, step);
    this->x += dx;
    this->y += dy;
    if (dx != 0) {
        this->direction = dx > 0 ? GameObject::RIGHT : GameObject::LEFT;
    }

    if (this->x != targetX || this->y != targetY) {
        return;
    }

    switch (rand() % 4) {
    case 2:
        this->diamondDir = -this->diamondDir;
        break;
    case 3:
        // Los vertices 0 y 2 estan sobre el eje X y los 1 y 3 sobre el eje Y
        this->furyPattern = Enemy::CROSS;
        if (this->diamondCorner % 2 == 0) {
            this->axis = Enemy::X;
            this->direction = this->x > static_cast<float>(this->originX) ? GameObject::LEFT : GameObject::RIGHT;
        } else {
            this->axis = Enemy::Y;
            this->verticalChange = this->y < static_cast<float>(this->originY);
        }
        this->atOrigin = false;
        return;
    default:
        break;
    }
    this->diamondCorner = (this->diamondCorner + this->diamondDir + 4) % 4;
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
