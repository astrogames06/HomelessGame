#ifndef GAME_H
#define GAME_H

#include <raylib.h>
#include <vector>
#include "../Entity/Entity.hpp"

class Entity;

struct Homeless
{
    Texture2D texture;
    Vector2 pos;
};

enum SCENE
{
    MENU,
    GAME,
    SCORE
};

class Game
{
public:
    int WIDTH = 850;
    int HEIGHT = 450;
    Texture2D map_tex;
    SCENE scene = MENU;
    std::vector<std::reference_wrapper<Entity>> entities;
    std::vector<Rectangle> obstacles;
    std::vector<Homeless> homeless_people;

    Camera2D camera;
    float timer = 60.f;
    int people_helped = 0;

    void Init();
    void SetUp(); // Sets up objects and entities

    void Update();
    void DrawUI();
    void Draw();
};

#endif