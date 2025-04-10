#include "Game.hpp"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <raygui.h>
#include <raymath.h>

#include "../Player/Player.hpp"
#include "../assets/map_png.h"

#include "../assets/guy_0_png.h"
#include "../assets/guy_1_png.h"
#include "../assets/guy_2_png.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>

    extern "C"
    {
        int high_score;
        EMSCRIPTEN_KEEPALIVE
        int GetHighScore()
        {
            return high_score;
        }

        EMSCRIPTEN_KEEPALIVE
        void SetHighScore(int x)
        {
            high_score = x;
        }
    }

    EM_JS(void, SaveHighScore, (int  x), {
        localStorage.setItem("high_score", x);
    });
#endif

Image map_img;

std::vector<Texture2D> guys_tex;

void Game::Init()
{
    map_img.data = MAP_PNG_DATA;
    map_img.format = MAP_PNG_FORMAT;
    map_img.mipmaps = 1;
    map_img.width = MAP_PNG_WIDTH;
    map_img.height = MAP_PNG_HEIGHT;
    map_tex = LoadTextureFromImage(map_img);

    for (Entity& entity : entities)
    {
        entity.Init();
    }

    Image guy_0_img;
    guy_0_img.data = GUY_0_PNG_DATA;
    guy_0_img.format = GUY_0_PNG_FORMAT;
    guy_0_img.mipmaps = 1;
    guy_0_img.width = GUY_0_PNG_WIDTH;
    guy_0_img.height = GUY_0_PNG_HEIGHT;

    Image guy_1_img;
    guy_1_img.data = GUY_1_PNG_DATA;
    guy_1_img.format = GUY_1_PNG_FORMAT;
    guy_1_img.mipmaps = 1;
    guy_1_img.width = GUY_1_PNG_WIDTH;
    guy_1_img.height = GUY_1_PNG_HEIGHT;

    Image guy_2_img;
    guy_2_img.data = GUY_2_PNG_DATA;
    guy_2_img.format = GUY_2_PNG_FORMAT;
    guy_2_img.mipmaps = 1;
    guy_2_img.width = GUY_2_PNG_WIDTH;
    guy_2_img.height = GUY_2_PNG_HEIGHT;

    guys_tex = {
        LoadTextureFromImage(guy_0_img),
        LoadTextureFromImage(guy_1_img),
        LoadTextureFromImage(guy_2_img)
    };
}

Player plr;
void Game::SetUp()
{
    camera = { 0 };
    camera.offset = (Vector2){ (float)WIDTH / 2, (float)HEIGHT / 2 };
    camera.target = plr.position;
    camera.zoom = 1.0f;
    obstacles.push_back({
        82, 83, 186, 154
    });
    obstacles.push_back({
        352, 83, 186, 154
    });
    obstacles.push_back({
        632, 83, 186, 154
    });
    obstacles.push_back({
        82, 320, 186, 154
    });
    obstacles.push_back({
        357, 320, 186, 154
    });
    obstacles.push_back({
        632, 320, 186, 154
    });

	plr.position = Vector2 { (float)WIDTH / 2, (float)26 };
	plr.Init();

}

float milli_delay = 0;
float second_delay;

float guy_spawn_delay = 0;

void Game::Update()
{
    if (scene == GAME)
    {
        milli_delay += GetFrameTime() * 1000.0f;  // GetFrameTime() returns time in seconds

        if (milli_delay >= 1.0f) {
            timer -= milli_delay / 1000.0f; // Convert ms to seconds
            milli_delay = 0.0f; // Reset after applying
        }

        if (timer < 0.0f) timer = 0.0f; // Prevent negative timer

        // End of game
        if (timer <= 0.0f)
        {
            scene = SCORE;
        }

        guy_spawn_delay++;
        if (guy_spawn_delay >= GetFPS()*3) {
            Texture2D tex = guys_tex[GetRandomValue(0,2)];

            Vector2 spawn_position;
            bool is_colliding;
            
            do {
                spawn_position = {(float)GetRandomValue(0, WIDTH), (float)GetRandomValue(0, HEIGHT)};
                is_colliding = false;
                
                // Check collision with obstacles
                for (const Rectangle& obstacle : obstacles) {
                    if (CheckCollisionRecs({spawn_position.x, spawn_position.y, (float)tex.width, (float)tex.height}, obstacle)) { 
                        is_colliding = true;
                        break;
                    }
                }
                
                // Check collision with other homeless people
                if (!is_colliding) {
                    for (const Homeless& h : homeless_people) {
                        if (CheckCollisionRecs({spawn_position.x, spawn_position.y, (float)tex.width, (float)tex.height}, 
                                            {h.pos.x, h.pos.y, (float)h.texture.width, (float)h.texture.height})) {
                            is_colliding = true;
                            break;
                        }
                    }
                }
            } while (is_colliding);
            
            homeless_people.push_back(Homeless{tex, spawn_position});
            guy_spawn_delay = 0;
        }


        camera.target.x = Clamp(plr.position.x, WIDTH / 2, 900 - WIDTH / 2);
        camera.target.y = Clamp(plr.position.y, HEIGHT / 2, 557 - HEIGHT / 2);

        for (Entity& entity : entities)
        {
            entity.Update();
        }

        for (int i = 0; i < homeless_people.size(); i++) {
            Homeless& h = homeless_people[i];
            
            if (IsKeyPressed(KEY_E) && CheckCollisionCircleRec(h.pos, h.texture.width+h.texture.height,
                {
                    (float)plr.position.x,
                    (float)plr.position.y,
                    (float)plr.texture.width,
                    (float)plr.texture.height
                }
            ))
            {
                homeless_people.erase(homeless_people.begin() + i);
                people_helped++;
            }
        }
    }
}

void Game::Reset()
{
    scene = GAME;
    people_helped = 0;
    timer = time_limit;
    homeless_people.clear();
    plr.position = Vector2 { (float)WIDTH / 2, (float)26 };
}

void Game::DrawUI()
{
    #if defined(PLATFORM_WEB)
        std::cout << "SCORE: = " << GetHighScore() << '\n';
    #endif
    if (scene == GAME)
    {
        std::ostringstream stream;
        stream << std::fixed << std::setprecision(2) << (std::round(timer * 100) / 100);
        std::string timer_str = "Timer: " + stream.str();
        DrawRectangleRounded({20-5, 20, (float)MeasureText(timer_str.c_str(), 20)+10, 20}, 0.5f, 10.f, LIGHTGRAY);
        DrawText(timer_str.c_str(), 20, 20, 20, BLACK);

        std::string score_str = "Score: " + std::to_string(people_helped);
        DrawRectangleRounded({200-5, 20, (float)MeasureText(score_str.c_str(), 20)+10, 20}, 0.5f, 10.f, LIGHTGRAY);
        DrawText(score_str.c_str(), 200, 20, 20, BLACK);
    }
    else if (scene == MENU)
    {
            std::string title = "Homeless Game";
            DrawText(title.c_str(), WIDTH/2-MeasureText(title.c_str(), 20)/2, 20, 20, BLACK);
            std::string credits = "Programming By Jesse & Art By Cy";
            DrawText(credits.c_str(), WIDTH/2-MeasureText(credits.c_str(), 20)/2, 40, 20, BLACK);

            #if defined(PLATFORM_WEB)
                std::string format_high_score = "High Score: " + std::to_string(GetHighScore());
                float highscore_width = (float)MeasureText(format_high_score.c_str(), 20)+10;
                DrawRectangleRounded({(WIDTH/2-highscore_width/2)-5, 80, highscore_width, 20}, 0.5f, 10.f, LIGHTGRAY);
                DrawText(format_high_score.c_str(), WIDTH/2-highscore_width/2, 80, 20, BLACK);
            #endif


        if (GuiButton(Rectangle{(float)WIDTH/2-150/2, (float)(HEIGHT/2-80/2)-50, 150, 80}, "PLAY!"))
        {
            Game::Reset();
        }
        if (GuiButton(Rectangle{(float)WIDTH/2-150/2, (float)(HEIGHT/2-80/2)+50, 150, 80}, "HOW TO PLAY"))
        {
            scene = HOW;
        }
        if (GuiButton(Rectangle{(float)WIDTH/2-150/2, (float)(HEIGHT/2-80/2)+150, 150, 80}, "INFO ON HOMELESSNESS"))
        {
            scene = INFO;
        }
    }
    else if (scene == SCORE)
    {
        std::string format_score = "Score: " + std::to_string(people_helped);
        float score_width = (float)MeasureText(format_score.c_str(), 20)+10;
        DrawRectangleRounded({(WIDTH/2-score_width/2)-5, 20, score_width, 20}, 0.5f, 10.f, LIGHTGRAY);
        DrawText(format_score.c_str(), WIDTH/2-score_width/2, 20, 20, BLACK);

        
        #if defined(PLATFORM_WEB)

            if (people_helped > high_score) {
                SetHighScore(people_helped);
                SaveHighScore(people_helped);
            }

            std::string format_high_score = "High Score: " + std::to_string(GetHighScore());
            float highscore_width = (float)MeasureText(format_high_score.c_str(), 20)+10;
            DrawRectangleRounded({(WIDTH/2-highscore_width/2)-5, 50, highscore_width, 20}, 0.5f, 10.f, LIGHTGRAY);
            DrawText(format_high_score.c_str(), WIDTH/2-highscore_width/2, 50, 20, BLACK);
        #endif

        if (GuiButton(Rectangle{(float)WIDTH/2-150/2, (float)100, 150, 80}, "PLAY AGAIN!"))
        {
            Game::Reset();
        }
        if (GuiButton(Rectangle{(float)WIDTH/2-150/2, (float)200, 150, 80}, "MENU"))
        {
            scene = MENU;
        }
    }
    else if (scene == INFO)
    {
        DrawText("Homelessness Information", WIDTH / 2 - MeasureText("Homelessness Information", 40) / 2, 20, 40, BLACK);

        DrawText(
            "Homelessness is a global problem affecting over 150 million people,",
            WIDTH / 2 - MeasureText("Homelessness is a global problem affecting over 150 million people,", 20) / 2,
            70, 20, BLACK
        );

        DrawText(
            "which is about 2% of the world's population.",
            WIDTH / 2 - MeasureText("which is about 2% of the world's population.", 20) / 2,
            100, 20, BLACK
        );

        DrawText(
            "The causes include job loss, domestic violence,",
            WIDTH / 2 - MeasureText("The causes include job loss, domestic violence,", 20) / 2,
            130, 20, BLACK
        );

        DrawText(
            "mental health issues, drug addiction, and lack of support.",
            WIDTH / 2 - MeasureText("mental health issues, drug addiction, and lack of support.", 20) / 2,
            160, 20, BLACK
        );

        DrawText(
            "Other factors include natural disasters, health problems,",
            WIDTH / 2 - MeasureText("Other factors include natural disasters, health problems,", 20) / 2,
            190, 20, BLACK
        );

        DrawText(
            "lack of ID, and military service.",
            WIDTH / 2 - MeasureText("lack of ID, and military service.", 20) / 2,
            220, 20, BLACK
        );

        DrawText(
            "A major cause is the lack of affordable housing.",
            WIDTH / 2 - MeasureText("A major cause is the lack of affordable housing.", 20) / 2,
            250, 20, BLACK
        );

        DrawText(
            "Solutions include shelter programs, job training,",
            WIDTH / 2 - MeasureText("Solutions include shelter programs, job training,", 20) / 2,
            280, 20, BLACK
        );

        DrawText(
            "mental health support, rehab, and donations.",
            WIDTH / 2 - MeasureText("mental health support, rehab, and donations.", 20) / 2,
            310, 20, BLACK
        );

        DrawText(
            "This game focuses on helping the homeless by donating,",
            WIDTH / 2 - MeasureText("This game focuses on helping the homeless by donating,", 20) / 2,
            340, 20, BLACK
        );

        DrawText(
            "providing clothes, securing ID, and ensuring access to food.",
            WIDTH / 2 - MeasureText("providing clothes, securing ID, and ensuring access to food.", 20) / 2,
            370, 20, BLACK
        );

        if (GuiButton(
            Rectangle{(float)WIDTH / 2 - 120 / 2, 400, 120, 45},
            "#185# Home"
        ))
        {
            scene = MENU;
        }
    }
    else if (scene == HOW)
    {
        DrawText("How to play", WIDTH/2-MeasureText("How to play", 40)/2, 50, 40, BLACK);
        DrawText(
            "To play the game you must go up to every homeless person you see,",
            WIDTH/2-MeasureText("To play the game you must go up to every homeless person you see,", 20)/2,
            150, 20, BLACK
        );
        DrawText(
            "and donate money to him. To donate money you must go up close",
            WIDTH/2-MeasureText("and donate money to him. To donate money you must go up close", 20)/2,
            180, 20, BLACK
        );
        DrawText(
            "to him and press the E key.",
            WIDTH/2-MeasureText("to him and press the E key.", 20)/2,
            210, 20, BLACK
        );
        DrawText(
            "Try and donate to as many people as possible",
            WIDTH/2-MeasureText("Try and donate to as many people as possible", 20)/2,
            240, 20, BLACK
        );
        DrawText(
            "before the 60 second timer runs out.",
            WIDTH/2-MeasureText("before the 60 second timer runs out.", 20)/2,
            270, 20, BLACK
        );

        if (GuiButton(
            Rectangle{(float)WIDTH / 2 - 120 / 2, 300, 120, 45},
            "#185# Home"
        ))
        {
            scene = MENU;
        }
    }
}

void Game::Draw()
{
    if (scene == GAME)
    {
        DrawTexture(map_tex, 0, 0, WHITE);
        for (Entity& entity : entities)
        {
            entity.Draw();
        }
        for (Homeless& homeless : homeless_people)
        {
            DrawTexture(homeless.texture, homeless.pos.x, homeless.pos.y, WHITE);
            //DrawCircleLines(homeless.pos.x+homeless.texture.width/2, homeless.pos.y+homeless.texture.height/2, homeless.texture.width+homeless.texture.height, GREEN);
        }
        // for (Rectangle& rec : obstacles)
        // {
        //     DrawRectangleLines(rec.x, rec.y, rec.width, rec.height, RED);
        // }
    }
}