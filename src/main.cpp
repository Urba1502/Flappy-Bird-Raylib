#include "raylib.h"

#define MAX_PIPES 3

struct Bird 
{
    float x;
    float y;
    float radius;
    float speedY;
};

struct Pipe 
{
    float x;
    float topheight;
    float bottomheight;
    float width;
    float speedX;
    bool passed;
};

int main()
{
    // Initialization
    int screenwidth = 800;
    int screenheight = 450;
    InitWindow(screenwidth, screenheight, "My Flappy Bird");

    // AUDIO: Initialize the audio device
    InitAudioDevice(); 

    SetTargetFPS(60); 

    // Load Textures
    Texture2D background = LoadTexture("C:\\Users\\Talha\\Desktop\\Flappy Bird\\bg.png");
    Texture2D birdTexture = LoadTexture("C:\\Users\\Talha\\Desktop\\Flappy Bird\\bird.png");

    // AUDIO: Load your sound files (adjust paths if necessary)
    Sound jumpSound = LoadSound("C:\\Users\\Talha\\Desktop\\Flappy Bird\\jump.wav");
    Sound crashSound = LoadSound("C:\\Users\\Talha\\Desktop\\Flappy Bird\\Blip.wav");

    int score = 0;
    bool isGameOver = false;

    float backgroundX = 0.0f;
    float backgroundspeed = 90.0f; 
    const float pipeGap = 130.0f; 

    Bird bird;
    bird.x = 250; 
    bird.y = 225;
    bird.radius = 15;
    bird.speedY = 0;

    Pipe pipes[MAX_PIPES];
    for (int i = 0; i < MAX_PIPES; i++) {
        pipes[i].x = 800.0f + (i * 300.0f); 
        pipes[i].topheight = (float)GetRandomValue(50, 250);
        pipes[i].bottomheight = (float)screenheight - pipeGap - pipes[i].topheight;
        pipes[i].width = 60; 
        pipes[i].speedX = 240.0f; 
        pipes[i].passed = false;
    }

    // Game Loop
    while(!WindowShouldClose())
    {
        float dt = GetFrameTime(); 

        if(!isGameOver)
        {
            // Jump handling
            if(IsKeyPressed(KEY_SPACE))
            {
                bird.speedY = -330.0f; 
                
                // AUDIO: Play the jump sound when spacebar is pressed
                PlaySound(jumpSound); 
            }

            // Apply Gravity
            float gravity = 980.0f; 
            bird.speedY += gravity * dt;
            bird.y += bird.speedY * dt;

            // Handle Pipes Loop
            for (int i = 0; i < MAX_PIPES; i++) 
            {
                pipes[i].x -= pipes[i].speedX * dt;

                if(bird.x > pipes[i].x + pipes[i].width && !pipes[i].passed)
                {
                    score++;
                    pipes[i].passed = true;
                }

                if(pipes[i].x < -pipes[i].width)
                {
                    float maxX = 0;
                    for(int j = 0; j < MAX_PIPES; j++) {
                        if(pipes[j].x > maxX) maxX = pipes[j].x;
                    }
                    
                    pipes[i].x = maxX + 300.0f; 
                    pipes[i].passed = false;
                    pipes[i].topheight = (float)GetRandomValue(50, (int)(screenheight - pipeGap - 50));
                    pipes[i].bottomheight = (float)screenheight - pipeGap - pipes[i].topheight;
                }

                Rectangle topPipeRect = {pipes[i].x - 4, 0, pipes[i].width + 8, pipes[i].topheight};
                Rectangle bottomPipeRect = {pipes[i].x - 4, (float)screenheight - pipes[i].bottomheight, pipes[i].width + 8, pipes[i].bottomheight};

                // Collision checking
                if(CheckCollisionCircleRec({bird.x, bird.y}, bird.radius, topPipeRect) || 
                   CheckCollisionCircleRec({bird.x, bird.y}, bird.radius, bottomPipeRect)) 
                {
                    if (!isGameOver) 
                    {
                        PlaySound(crashSound);
                    }
                    isGameOver = true;
                }
            }

            // Ceiling and Floor boundaries
            if (bird.y + bird.radius > (float)screenheight || bird.y - bird.radius < 0) 
            {
                if (!isGameOver) 
                {
                    PlaySound(crashSound);
                }
                isGameOver = true;
            }

            // Background math
            backgroundX -= backgroundspeed * dt;
            if(backgroundX <= -background.width)
            {
                backgroundX = 0.0f;
            }
        }
        else
        {
            // Reset state
            if(IsKeyPressed(KEY_SPACE))
            {
                bird.x = 250; 
                bird.y = 225;
                bird.speedY = 0;
                
                for (int i = 0; i < MAX_PIPES; i++) {
                    pipes[i].x = 800.0f + (i * 300.0f);
                    pipes[i].topheight = (float)GetRandomValue(50, 250);
                    pipes[i].bottomheight = (float)screenheight - pipeGap - pipes[i].topheight;
                    pipes[i].passed = false;
                }

                score = 0;
                isGameOver = false;
            }
        }

        // Drawing
        BeginDrawing();
            ClearBackground(SKYBLUE);
            
            if (background.id > 0) 
            {
                float currentX = backgroundX;
                while (currentX < screenwidth)
                {
                    DrawTexture(background, currentX, 0, WHITE);
                    currentX += background.width; 
                }
            }

            for (int i = 0; i < MAX_PIPES; i++) 
            {
                DrawRectangle(pipes[i].x, 0, pipes[i].width, pipes[i].topheight, GetColor(0x00a800ff));
                DrawRectangle(pipes[i].x + pipes[i].width - 10, 0, 10, pipes[i].topheight, DARKGREEN);
                DrawRectangle(pipes[i].x + 4, 0, 6, pipes[i].topheight, LIME);
                DrawRectangle(pipes[i].x - 4, pipes[i].topheight - 24, pipes[i].width + 8, 24, GetColor(0x007300ff));
                DrawRectangleLines(pipes[i].x - 4, pipes[i].topheight - 24, pipes[i].width + 8, 24, BLACK);

                DrawRectangle(pipes[i].x, (float)screenheight - pipes[i].bottomheight, pipes[i].width, pipes[i].bottomheight, GetColor(0x00a800ff));
                DrawRectangle(pipes[i].x + pipes[i].width - 10, (float)screenheight - pipes[i].bottomheight, 10, pipes[i].bottomheight, DARKGREEN);
                DrawRectangle(pipes[i].x + 4, (float)screenheight - pipes[i].bottomheight, 6, pipes[i].bottomheight, LIME);
                DrawRectangle(pipes[i].x - 4, (float)screenheight - pipes[i].bottomheight, pipes[i].width + 8, 24, GetColor(0x007300ff));
                DrawRectangleLines(pipes[i].x - 4, (float)screenheight - pipes[i].bottomheight, pipes[i].width + 8, 24, BLACK);
            }

            if(birdTexture.id>0)
            {
                Rectangle sourceRect = {0.0f,0.0f,(float)birdTexture.width,(float)birdTexture.height};

                float displayWidth = 60.0f;
                float displayHeight = 50.0f;

                Rectangle destRect = {bird.x,bird.y,displayWidth,displayHeight};

                Vector2 origin = {displayWidth / 2.0f , displayHeight / 2.0f };

                float rotationAngle = bird.speedY * 0.12f;

                if(rotationAngle<-20.0f) rotationAngle = -20.0f;
                if(rotationAngle>70.0f) rotationAngle = 70.0f;

                DrawTexturePro(birdTexture,sourceRect,destRect,origin,rotationAngle,WHITE);
            }

            if(!isGameOver)
            {
                DrawText(TextFormat("SCORE: %i", score), 20, 20, 40, WHITE);
            }
            else
            {
                DrawRectangle(0, 0, screenwidth, screenheight, Fade(BLACK, 0.4f));
                DrawText("GAMEOVER", screenwidth / 2 - 140, 120, 50, RED);
                DrawText(TextFormat("FINAL SCORE: %i", score), screenwidth / 2 - 110, 200, 30, WHITE);
                DrawText("PRESS SPACE TO RESTART!", screenwidth / 2 - 160, 280, 25, LIGHTGRAY);
            }
        EndDrawing();
    }

    // De-initialization
    UnloadTexture(background);
    UnloadTexture(birdTexture);
    
    // AUDIO: Unload audio from memory and close the system
    UnloadSound(jumpSound);
    UnloadSound(crashSound);
    CloseAudioDevice(); 

    CloseWindow();
    return 0;
}

/*

C:\raylib\w64devkit\bin\g++.exe main.cpp -o flappy.exe -O1 -Wall -I C:\raylib\raylib\src -L C:\raylib\raylib\src -lraylib -lopengl32 -lgdi32 -lwinmm

*/