#pragma once
#include <string>
#include <vector>

// Objeto a colocar en un nivel, tal como se lee del archivo
struct Spawn
{
    enum Kind { ENEMY, ITEM, BOSS };
    Kind kind = ENEMY;
    float x = 0, y = 0, width = 0, height = 0;
    float hits = 0; // Enemigo y jefe
    int speed = 0, distance = 0, direction = 0, axis = 0; // Enemigo y jefe (axis solo enemigo)
    int itemType = 0; // Item
};

// Datos de un nivel: mapa, posicion inicial del heroe y objetos a colocar
struct LevelData
{
    std::string mapFile, collisionFile;
    float heroX = 0, heroY = 0;
    std::vector<Spawn> spawns;
};

// Lee un archivo de nivel; lanza std::runtime_error("archivo:linea: mensaje") si es invalido
LevelData loadLevel(const std::string &path);
