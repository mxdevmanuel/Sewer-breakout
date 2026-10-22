#include "hero.h"

Hero::Hero(float x, float y, float width, float height, int lives, int speed): GameObject(x, y, width, height)
{
    this->collisionMap = nullptr;
    this->current = nullptr;
    this->lives = lives;
    this->speed = speed;
    this->points = 0;
    this->type = GameObject::player;
    this->loadSpriteSheet("Sprites/OSAS.png");
    this->lastDirection = this->RIGHT;
    this->lastPosition = this->STANDING;
    this->moving = false;
	this->nextLevel = false;
    Hero::setSprites();
}

void Hero::setSprites()
{
    this->positions.insert(std::pair<int, Sprite>(this->STANDING, Sprite{93, 33, this->width, this->height}));
    this->positions.insert(std::pair<int, Sprite>(this->WALKING, Sprite{92, 0, this->width, this->height}));
}

void Hero::move(int direction)
{
    if (!this->directions[direction]) {
        this->directions[direction] = true;
    }
}

void Hero::stop(int direction)
{
    if (this->directions[direction]) {
        this->directions[direction] = false;
    }
}

void Hero::draw()
{
    currentPosition = &positions[lastPosition];
    al_draw_bitmap_region(this->spriteSheet,
                          currentPosition->sx,
                          currentPosition->sy,
                          currentPosition->sw,
                          currentPosition->sh,
                          this->getX(),
                          this->getY(),
                          lastDirection == this->LEFT ? ALLEGRO_FLIP_HORIZONTAL : 0);
}

int Hero::getLastDirection()
{
    return this->lastDirection;
}

void Hero::update()
{
    if (this->isBoundary()) {
        if (directions[0]) {
            directions[0] = false;
            this->x -= static_cast<float>(this->speed * 2);
            this->lastDirection = GameObject::RIGHT;
            this->lastPosition = Hero::STANDING;
        }

        if (directions[1]) {
            directions[1] = false;
            this->x += static_cast<float>(this->speed);
            this->lastDirection = GameObject::LEFT;
            this->lastPosition = Hero::STANDING;
        }

        if (directions[2]) {
            directions[2] = false;
            this->y -= static_cast<float>(this->speed);
            this->lastPosition = Hero::STANDING;
        }

        if (directions[3]) {
            directions[3] = false;
            this->y += static_cast<float>(this->speed * 2);
            this->lastPosition = Hero::STANDING;
        }

    } else {

        if (directions[0]) {
            this->x += static_cast<float>(this->speed);
            this->lastDirection = GameObject::RIGHT;
            this->lastPosition = Hero::WALKING;
        }

        if (directions[1]) {
            this->x -= static_cast<float>(this->speed);
            this->lastDirection = GameObject::LEFT;
            this->lastPosition = Hero::WALKING;
        }

        if (directions[2]) {
            this->y += static_cast<float>(this->speed);
            this->lastPosition = Hero::WALKING;
        }

        if (directions[3]) {
            this->y -= static_cast<float>(this->speed);
            this->lastPosition = Hero::WALKING;
        }

        if (!directions[0] && !directions[1] && !directions[2] && !directions[3]) {
            this->lastPosition = Hero::STANDING;
        }

    }
}

bool Hero::isBoundary()
{
    this->currentColors[0] = al_get_pixel(this->collisionMap, static_cast<int>(this->getX()), static_cast<int>(this->getY()));
    this->currentColors[1] = al_get_pixel(this->collisionMap, static_cast<int>(this->getX() + this->getWidth()), static_cast<int>(this->getY() + this->getHeight()));

	if (currentColors[0].b == 1 && currentColors[1].b == 1)
	{
		this->nextLevel = true;
	}
	else {
		this->nextLevel = false;
	}

    if ((currentColors[0].r == 0 && currentColors[0].g == 0 && currentColors[0].b == 0) || (currentColors[1].r == 0 && currentColors[1].g == 0 && currentColors[1].b == 0)) {
        return true;
    } else {
        return false;
    }
}

void Hero::updateMap(Mapa *current)
{
    this->current = current;
    this->collisionMap = current->getCollisionMap();
}

bool Hero::isDead()
{
    return this->lives < 0;
}

bool Hero::mustStartAgain()
{
    if (this->restart) {
        this->restart = false;
        return true;
    }

    return false;
}

void Hero::die()
{
    if (this->lives >= 0) {
        this->lives--;
        this->restart = true;
    }
}

void Hero::collision(GameObject* cw)
{
    switch(cw->getType())
    {
        case GameObject::enemy:
        case GameObject::enemy_missile:
            die();
            break;
        case GameObject::item:
            this->lives++;
            break;
        default:
            break;
    }
}

int Hero::getPoints()
{
    return this->points;
}

void Hero::setPoints(int points)
{
	this->points = points;
}

int Hero::getLives()
{
    return this->lives;
}

void Hero::setItemList()
{

}
