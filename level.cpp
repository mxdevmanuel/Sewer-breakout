#include "level.h"
#include "enemy.h"
#include "item.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace {

struct Name
{
    const char *text;
    int value;
};

constexpr Name directionNames[] = {{"LEFT", GameObject::LEFT}, {"RIGHT", GameObject::RIGHT}};
constexpr Name axisNames[] = {{"X", Enemy::X}, {"Y", Enemy::Y}};
constexpr Name itemNames[] = {{"SHOVEL", Item::SHOVEL}, {"POTION", Item::POTION}, {"GEM", Item::GEM}, {"LAMP", Item::LAMP}};

// Lee un nombre de la linea y lo traduce con `names`
template <std::size_t N>
int readName(std::istringstream &line, const Name (&names)[N], const char *field)
{
    std::string text;
    line >> text;
    for (const Name &name : names) {
        if (text == name.text) {
            return name.value;
        }
    }
    throw std::invalid_argument(std::string("valor invalido para ") + field + ": '" + text + "'");
}

} // namespace

// Formato: una entidad por linea, `#` inicia un comentario
//   map   <imagen> <imagen de colision>
//   hero  x y
//   enemy x y ancho alto golpes velocidad distancia LEFT|RIGHT X|Y
//   item  x y ancho alto SHOVEL|POTION|GEM|LAMP
//   boss  x y ancho alto golpes velocidad distancia LEFT|RIGHT
LevelData loadLevel(const std::string &path)
{
    std::ifstream file(path);
    if (!file) {
        throw std::runtime_error(path + ": no se pudo abrir el archivo");
    }

    LevelData level;
    bool hasMap = false, hasHero = false;
    std::string text;
    for (int lineNumber = 1; std::getline(file, text); ++lineNumber) {
        text = text.substr(0, text.find('#'));
        std::istringstream line(text);
        std::string keyword;
        if (!(line >> keyword)) {
            continue; // Linea vacia o comentario
        }

        try {
            Spawn spawn;
            if (keyword == "map") {
                line >> level.mapFile >> level.collisionFile;
                hasMap = true;
            } else if (keyword == "hero") {
                line >> level.heroX >> level.heroY;
                hasHero = true;
            } else if (keyword == "enemy") {
                spawn.kind = Spawn::ENEMY;
                line >> spawn.x >> spawn.y >> spawn.width >> spawn.height >> spawn.hits >> spawn.speed >> spawn.distance;
                spawn.direction = readName(line, directionNames, "direccion");
                spawn.axis = readName(line, axisNames, "eje");
            } else if (keyword == "item") {
                spawn.kind = Spawn::ITEM;
                line >> spawn.x >> spawn.y >> spawn.width >> spawn.height;
                spawn.itemType = readName(line, itemNames, "item");
            } else if (keyword == "boss") {
                spawn.kind = Spawn::BOSS;
                line >> spawn.x >> spawn.y >> spawn.width >> spawn.height >> spawn.hits >> spawn.speed >> spawn.distance;
                spawn.direction = readName(line, directionNames, "direccion");
            } else {
                throw std::invalid_argument("palabra clave desconocida: '" + keyword + "'");
            }

            std::string extra;
            if (line.fail() || line >> extra) {
                throw std::invalid_argument("campos faltantes o invalidos en '" + keyword + "'");
            }
            if (keyword != "map" && keyword != "hero") {
                level.spawns.push_back(spawn);
            }
        } catch (const std::invalid_argument &e) {
            throw std::runtime_error(path + ":" + std::to_string(lineNumber) + ": " + e.what());
        }
    }

    if (!hasMap || !hasHero) {
        throw std::runtime_error(path + ": faltan las lineas 'map' o 'hero'");
    }
    return level;
}
