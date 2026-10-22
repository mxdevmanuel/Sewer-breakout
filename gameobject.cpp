#include "gameobject.h"

GameObject::GameObject(float x, float y, float width, float height)
{
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
}

void GameObject::loadSpriteSheet(std::string fileName)
{
    this->spriteSheet = al_load_bitmap(fileName.c_str());
    if (this->spriteSheet == NULL) {
        al_show_native_message_box(NULL, "Error!", "Fallo la carga de la imagen", fileName.c_str(), NULL, ALLEGRO_MESSAGEBOX_ERROR);
        throw 1;
    }
}

float GameObject::getX()
{
    return this->x;
}

float GameObject::getY()
{
    return this->y;
}

float GameObject::getWidth()
{
    return this->width;
}

float GameObject::getHeight()
{
    return this->height;
}

int GameObject::getId()
{
    return this->id;
}

int GameObject::getType()
{
    return this->type;
}

bool GameObject::isMoving()
{
    return this->moving;
}

void GameObject::getBounds(float *x, float *y, float *finalX, float *finalY)
{
    *x = this->x;
    *y = this->y;
    *finalX = this->x + this->width;
    *finalY = this->y + this->height;
}

void GameObject::setPosition(float x, float y)
{
    this->x = x;
    this->y = y;
}
