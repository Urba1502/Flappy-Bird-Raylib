#include "raylib.h"
#ifdef PLATFORM_WEB
#include <emscripten/emscripten.h>
#endif

#define MAX_PIPES 3

struct Bird { float x, y, radius, speedY; };
struct Pipe { float x, topheight, bottomheight, width, speedX; bool passed; };

// Global game state
const int screenWidth = 1280;
const int screenHeight = 720;

Bird bird;
Pipe pipes[MAX_PIPES];

Texture2D background, birdTexture;
Sound jumpSound, crashSound;

bool isGameStarted = false;
bool isGameOver = false;
int score = 0;
float backgroundX = 0.0f;
float backgroundSpeed = 90.0f;
const float pipeGap = 130.0f;




// Helper function to set or reset the game elements
void ResetGame() 
{
    bird.x = 250; 
    bird.y = 225;
    bird.radius = 25.0f; 
    bird.speedY = 0;
    backgroundX = 0.0f;
    score = 0;
    isGameOver = false;
    isGameStarted = false; 

    for (int i = 0; i < MAX_PIPES; i++) {
        pipes[i].x = 800.0f + (i * 650.0f); // Spaced them out slightly better
        pipes[i].topheight = (float)GetRandomValue(50, (int)(screenHeight - pipeGap - 50));
        pipes[i].bottomheight = (float)screenHeight - pipeGap - pipes[i].topheight;
        pipes[i].width = 60; 
        pipes[i].speedX = 240.0f; 
        pipes[i].passed = false;
    }
}

void UpdateDrawFrame()
{
    float dt = GetFrameTime();
    if (dt > 0.1f) dt = 0.1f;

    bool flap =
    IsKeyPressed(KEY_SPACE) ||
    IsMouseButtonPressed(MOUSE_LEFT_BUTTON) ||
    IsGestureDetected(GESTURE_TAP);

    // STATE 1: Not started yet
    if (!isGameStarted)
    {

    if (flap)
        {
            isGameStarted = true;
            bird.speedY = -330.0f; // Give a slight jump boost right as they start!
            PlaySound(jumpSound);
        }
    }
    // STATE 2: Actively playing
    else if (!isGameOver)
    {
        // Jump handling
        if (flap)
        {
            bird.speedY = -330.0f; 
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

            if (bird.x > pipes[i].x + pipes[i].width && !pipes[i].passed)
            {
                score++;
                pipes[i].passed = true;
            }

            // RECYCLING PIPES (Deterministically calculating spacing based on the prior index)
            if (pipes[i].x < -pipes[i].width)
            {
                // Find the index of the pipe logically behind this one
                int prevPipeIndex = (i == 0) ? MAX_PIPES - 1 : i - 1;
                
                // Position this pipe exactly 350px ahead of the previous pipe's current position
                pipes[i].x = pipes[prevPipeIndex].x + 650.0f; 
                
                pipes[i].passed = false;
                pipes[i].topheight = (float)GetRandomValue(50, (int)(screenHeight - pipeGap - 50));
                pipes[i].bottomheight = (float)screenHeight - pipeGap - pipes[i].topheight;
            }

            Rectangle topPipeRect = { pipes[i].x - 4, 0, pipes[i].width + 8, pipes[i].topheight };
            Rectangle bottomPipeRect = { pipes[i].x - 4, (float)screenHeight - pipes[i].bottomheight, pipes[i].width + 8, pipes[i].bottomheight };

            // Collision checking
            if (CheckCollisionCircleRec({ bird.x, bird.y }, bird.radius, topPipeRect) || 
                CheckCollisionCircleRec({ bird.x, bird.y }, bird.radius, bottomPipeRect)) 
            {
                if (!isGameOver) 
                {
                    PlaySound(crashSound);
                }
                isGameOver = true;
            }
        }

        // Ceiling and Floor boundaries
        if (bird.y + bird.radius > (float)screenHeight || bird.y - bird.radius < 0) 
        {
            if (!isGameOver) 
            {
                PlaySound(crashSound);
            }
            isGameOver = true;
        }

        // Background math
        backgroundX -= backgroundSpeed * dt;
        if (backgroundX <= -background.width)
        {
            backgroundX = 0.0f;
        }
    }
    // STATE 3: Game Over
    else
    {
        // Reset state
        if(flap)
        {
            ResetGame();
        }
    }

    // Drawing code 
    BeginDrawing();
    ClearBackground(SKYBLUE);

    // Draw background
    if (background.id > 0) 
    {
        float currentX = backgroundX;
        while (currentX < screenWidth)
        {
            DrawTexture(background, currentX, 0, WHITE);
            currentX += background.width; 
        }
    }

    // Draw pipes
    for (int i = 0; i < MAX_PIPES; i++) 
    {
        DrawRectangle(pipes[i].x, 0, pipes[i].width, pipes[i].topheight, GetColor(0x00a800ff));
        DrawRectangle(pipes[i].x + pipes[i].width - 10, 0, 10, pipes[i].topheight, DARKGREEN);
        DrawRectangle(pipes[i].x + 4, 0, 6, pipes[i].topheight, LIME);
        DrawRectangle(pipes[i].x - 4, pipes[i].topheight - 24, pipes[i].width + 8, 24, GetColor(0x007300ff));
        DrawRectangleLines(pipes[i].x - 4, pipes[i].topheight - 24, pipes[i].width + 8, 24, BLACK);

        DrawRectangle(pipes[i].x, (float)screenHeight - pipes[i].bottomheight, pipes[i].width, pipes[i].bottomheight, GetColor(0x00a800ff));
        DrawRectangle(pipes[i].x + pipes[i].width - 10, (float)screenHeight - pipes[i].bottomheight, 10, pipes[i].bottomheight, DARKGREEN);
        DrawRectangle(pipes[i].x + 4, (float)screenHeight - pipes[i].bottomheight, 6, pipes[i].bottomheight, LIME);
        DrawRectangle(pipes[i].x - 4, (float)screenHeight - pipes[i].bottomheight, pipes[i].width + 8, 24, GetColor(0x007300ff));
        DrawRectangleLines(pipes[i].x - 4, (float)screenHeight - pipes[i].bottomheight, pipes[i].width + 8, 24, BLACK);
    }

    // Draw bird
    if (birdTexture.id > 0)
    {
        Rectangle sourceRect = { 0.0f, 0.0f, (float)birdTexture.width, (float)birdTexture.height };
        float displayWidth = 60.0f;
        float displayHeight = 50.0f;
        Rectangle destRect = { bird.x, bird.y, displayWidth, displayHeight };
        Vector2 origin = { displayWidth / 2.0f , displayHeight / 2.0f };

        float rotationAngle = bird.speedY * 0.12f;
        if (rotationAngle < -20.0f) rotationAngle = -20.0f;
        if (rotationAngle > 70.0f) rotationAngle = 70.0f;

        // When waiting to start, keep the bird level
        if (!isGameStarted) rotationAngle = 0.0f;

        DrawTexturePro(birdTexture, sourceRect, destRect, origin, rotationAngle, WHITE);
    }
    else 
    {
        // Fallback placeholder if texture isn't loaded
        DrawCircle(bird.x, bird.y, bird.radius, YELLOW);
    }

    // UI Rendering based on game state
    if (!isGameStarted)
    {
        DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.3f));
        DrawText("FLAPPY BIRD", screenWidth / 2 - 170, 120, 50, GOLD);
        DrawText("PRESS SPACE TO FLAP & START!", screenWidth / 2 - 210, 240, 25, WHITE);
    }
    else if (!isGameOver)
    {
        DrawText(TextFormat("SCORE: %i", score), 20, 20, 40, WHITE);
    }
    else
    {
        DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.4f));
        DrawText("GAMEOVER", screenWidth / 2 - 140, 120, 50, RED);
        DrawText(TextFormat("FINAL SCORE: %i", score), screenWidth / 2 - 110, 200, 30, WHITE);
        DrawText("PRESS SPACE TO RESTART!", screenWidth / 2 - 160, 280, 25, LIGHTGRAY);
    }
    
    EndDrawing();
}

int main()
{
    InitWindow(screenWidth,screenHeight,"Flappy Bird");
    SetWindowState(FLAG_WINDOW_RESIZABLE);

#ifdef PLATFORM_WEB
    // Directly inject the CSS styles into the webpage head to handle mobile scale
    emscripten_run_script(
        "var style = document.createElement('style');"
        "style.innerHTML = `"
        "  body, html {"
        "    margin: 0;"
        "    padding: 0;"
        "    width: 100%;"
        "    height: 100%;"
        "    overflow: hidden;"
        "    background-color: #000;"
        "    display: flex;"
        "    justify-content: center;"
        "    align-items: center;"
        "  }"
        "  canvas {"
        "    width: 100vw !important;"
        "    height: 56.25vw !important;"
        "    max-height: 100vh !important;"
        "    max-width: 177.78vh !important;"
        "    image-rendering: pixelated;"
        "    image-rendering: crisp-edges;"
        "    display: block;"
        "  }"
        "`;"
        "document.head.appendChild(style);"
    );
#else
    ToggleFullscreen();
#endif

    InitAudioDevice();

    // Load resources safely
    background = LoadTexture("assets/bg.png");
    birdTexture = LoadTexture("assets/bird.png");
    jumpSound = LoadSound("assets/Jump.wav");
    crashSound = LoadSound("assets/Blip.wav");

    SetTargetFPS(60);
    
    // Run the initial setups
    ResetGame();

#ifdef PLATFORM_WEB
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    while (!WindowShouldClose())
    {
        UpdateDrawFrame();
    }

#endif

    // Clean up
    UnloadTexture(background);
    UnloadTexture(birdTexture);
    UnloadSound(jumpSound);
    UnloadSound(crashSound);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}