/*******************************************************************************************
*
*   raylib [shapes] example - Draw basic shapes 2d (rectangle, circle, line...)
*
*   Example complexity rating: [★☆☆☆] 1/4
*
*   Example originally created with raylib 1.0, last time updated with raylib 4.2
*
*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2014-2025 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"

#if defined(__EMSCRIPTEN__)
#include <emscripten.h>
#endif

// EMSCRIPTEN ONLY!
// 
// At runtime, this function attempts to load a file
// from the provided URL, and maps it to emscripten's
// filesystem. You can then use the file in any C/C++
// filesystem function as if it were on the local disk.
void FILE_RESOLVE(const char* url, const char* file)
{
    #if defined(__EMSCRIPTEN__)
    emscripten_wget(url, file);
    emscripten_sleep(0);
    #endif
}

#include <iostream> // for cout
#include <stdlib.h> // for rand

void DrawStringDropShadow(const char* text, int x,  int y, int fontSize, Color foreground, Color background)
{
    DrawText(text, x + 1, y + 1, fontSize, background);
    DrawText(text, x, y, fontSize, foreground);
}

Color GetRandomColor()
{
    return Color(rand() % 128, rand() % 128, rand() % 128);
}

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;
    
    
    // raysan5/raylib/master/examples/shaders/resources/shaders/glsl100/pbr.vs
    // load "assets/gfx/broken.png" from a URL
    FILE_RESOLVE("https://raw.githubusercontent.com/PGEtinker/assets/main/broken.png", "assets/gfx/broken.png");

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "raylib [shapes] example - basic shapes drawing");

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------
    Vector2 mousePos;
    Color background = GetRandomColor();

    Image image = LoadImage("assets/gfx/broken.png");
    Texture2D texture = LoadTextureFromImage(image);
    UnloadImage(image);   // Once image has been converted to texture and uploaded to VRAM, it can be unloaded from RAM

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        mousePos = GetMousePosition();

        if(IsMouseButtonPressed(0))
        {
            background = GetRandomColor();
            std::cout << TextFormat("(%d,%d)\n", static_cast<int>(mousePos.x), static_cast<int>(mousePos.y));
        }
        //----------------------------------------------------------------------------------
        


        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(background);
            
            DrawRectangleLines(1, 1, GetScreenWidth()-2, GetScreenHeight()-2, YELLOW);
            DrawRectangleLines(2, 2, GetScreenWidth()-4, GetScreenHeight()-4, YELLOW);
            DrawRectangleLines(3, 3, GetScreenWidth()-6, GetScreenHeight()-6, YELLOW);

            DrawStringDropShadow(
                "Hello, raylib",
                10,
                10,
                (GetScreenHeight() / 100) * 4,
                WHITE,
                BLACK
            );
            
            DrawStringDropShadow(
                "Mouse position SHOULD match closely\nto the circle. Yellow borders should\nalways be visible.\n\nLEFT MOUSE to change color.",
                10,
                40 + ((GetScreenHeight() / 100) * 4),
                (GetScreenHeight() / 100) * 4,
                WHITE,
                BLACK
            );
            
            DrawStringDropShadow(
                TextFormat("(%d,%d)", static_cast<int>(mousePos.x), static_cast<int>(mousePos.y)),
                10,
                GetScreenHeight() - (20 + (GetScreenHeight() / 100) * 4),
                (GetScreenHeight() / 100) * 4,
                WHITE,
                BLACK
            );
            
            DrawTextureEx(
                texture,
                Vector2{
                    10,
                    (20 + (static_cast<float>(GetScreenHeight()) / 100) * 4) * 5
                },
                0.0f,
                2.0f,
                WHITE
            );

            // Circle shapes and lines
            DrawCircle(mousePos.x, mousePos.y, (static_cast<float>(GetScreenHeight()) / 100) * 2.0f, RED);
            DrawCircle(mousePos.x, mousePos.y, (static_cast<float>(GetScreenHeight()) / 100) * 0.3f, WHITE);
        
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}