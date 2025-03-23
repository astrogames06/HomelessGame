#include "Player.hpp"

#include <iostream>
#include <raymath.h>

#include "../assets/player_png.h"

float MOVE_SPEED = 20.0f;

extern Game game;

Image plr_img;
void Player::Init()
{
    Entity::Init();

    plr_img.data = PLAYER_PNG_DATA;
    plr_img.width = PLAYER_PNG_WIDTH;
    plr_img.height = PLAYER_PNG_HEIGHT;
    plr_img.format = PLAYER_PNG_FORMAT;
    plr_img.mipmaps = 1;
    texture = LoadTextureFromImage(plr_img);
    texture.width -= 10;
    texture.height -= 10;
}

void CheckCollisions(Player &player, bool horizontal, Texture2D& tex)
{
    for (const auto &rec : game.obstacles)
    {
        Rectangle playerRect = {player.position.x, player.position.y, (float)tex.width, (float)tex.height};
        if (CheckCollisionRecs(playerRect, rec))
        {
            if (horizontal)
            {
                if (player.velocity.x > 0)
                    player.position.x = rec.x - tex.width;
                else if (player.velocity.x < 0)
                    player.position.x = rec.x + rec.width;
            }
            else
            {
                if (player.velocity.y > 0)
                    player.position.y = rec.y - tex.height;
                else if (player.velocity.y < 0)
                    player.position.y = rec.y + rec.height;
            }
        }
    }
}

void Player::Update()
{
    velocity = {0, 0};
    
    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
    {
        velocity.y -= MOVE_SPEED * GetFrameTime();
    }
    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
    {
        velocity.y += MOVE_SPEED * GetFrameTime();
    }
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))
    {
        velocity.x -= MOVE_SPEED * GetFrameTime();
    }
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT))
    {
        velocity.x += MOVE_SPEED * GetFrameTime();
    }
    
    position.x += velocity.x;
    CheckCollisions(*this, true, texture);
    position.y += velocity.y;
    CheckCollisions(*this, false, texture);
    std::cout << position.x << " " << position.y << '\n';
    position = Vector2Clamp(position, {0, 0}, {875, 514});
}

void Player::Draw()
{
    DrawTextureV(texture, position, WHITE);
    //DrawRectangleLines(position.x, position.y, texture.width, texture.height, GREEN);
}