#ifndef PLAYER_H
#define PLAYER_H

#include "../Entity/Entity.hpp"

class Player : public Entity
{
public:
    Texture2D texture;
    Vector2 velocity;
    const int PLAYER_SIZE = 50;
    const float MOVE_SPEED = 200.0f;

    int donations = 0;

    void Init() override;
    void Update() override;
    void Draw() override;
};

#endif