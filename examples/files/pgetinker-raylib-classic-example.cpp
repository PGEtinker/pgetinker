/*******************************************************************************************
*
*   raylib example - PGEtinker Classic Example
*   
********************************************************************************************
*   
*   This is free and unencumbered software released into the public domain.
*   
*   Anyone is free to copy, modify, publish, use, compile, sell, or
*   distribute this software, either in source code form or as a compiled
*   binary, for any purpose, commercial or non-commercial, and by any
*   means.
*   
*   In jurisdictions that recognize copyright laws, the author or authors
*   of this software dedicate any and all copyright interest in the
*   software to the public domain. We make this dedication for the benefit
*   of the public at large and to the detriment of our heirs and
*   successors. We intend this dedication to be an overt act of
*   relinquishment in perpetuity of all present and future rights to this
*   software under copyright law.
*   
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
*   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
*   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
*   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
*   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
*   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
*   OTHER DEALINGS IN THE SOFTWARE.
*   
*   For more information, please refer to <https://unlicense.org>
*
********************************************************************************************/
#include "raylib.h"
#include "pgetinker.h"

#include <algorithm>
#include <iostream>
#include <stdlib.h>

//------------------------------------------------------------------------------------
// Calculates the rectangles used to scale the canvas
//------------------------------------------------------------------------------------
Rectangle CalculateCanvasDestination(float width, float height)
{
    float screenWidth = (float)GetScreenWidth();
    float screenHeight = (float)GetScreenHeight();

    float aspect = width / height;
    float screenAspect = screenWidth / screenHeight;
    float scale;
    
    Rectangle ret = {0,0,0,0};

    if(aspect > screenAspect)
    {
        scale = screenWidth / width;
        ret.width = screenWidth;
        ret.height = height * scale;
        ret.x = 0;
        ret.y = (screenHeight - ret.height) / 2;
    }
    else
    {
        scale = screenHeight / height;
        ret.width = width * scale;
        ret.height = screenHeight;
        ret.x = (screenWidth - ret.width) / 2;
        ret.y = 0;
    }
    
    return ret;
}
//------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------
// Get a randomized color of full alpha.
//------------------------------------------------------------------------------------
Color GetRandomColor()
{
    return Color(rand() % 128, rand() % 128, rand() % 128, 255);
}
//------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------
// Draw Text with a drop shadow of the provided foreground/background colors.
//------------------------------------------------------------------------------------
void DrawTextDropShadow(const char* text, int x,  int y, int fontSize, Color foreground, Color background)
{
    DrawText(text, x + 1, y + 1, fontSize, background);
    DrawText(text, x, y, fontSize, foreground);
}
//------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    const int canvasWidth = 256;
    const int canvasHeight = 240;

    // Initialization
    //--------------------------------------------------------------------------------------
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(canvasWidth, canvasHeight, "raylib - PGEtinker Classic Example");
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    
    RenderTexture2D canvas = LoadRenderTexture(canvasWidth, canvasHeight);
    SetTextureFilter(canvas.texture, TEXTURE_FILTER_POINT);

    pgetinker_file_resolve("https://raw.githubusercontent.com/PGEtinker/assets/main/broken.png", "assets/gfx/broken.png");
    Texture2D imageFromURL = LoadTexture("assets/gfx/broken.png");
    
    Color background = GetRandomColor();
    Vector2 realMousePosition = {0, 0};
    Vector2 mousePosition     = {0, 0};

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Core Update
        //----------------------------------------------------------------------------------

        // Calculate Canvas Position
        Rectangle canvasSourceRect = { 0, 0, canvasWidth, -canvasHeight };
        Rectangle canvasDestinationRect = CalculateCanvasDestination(canvasWidth, canvasHeight);
        
        // Calculate mouse position 
        realMousePosition = GetMousePosition();
        
        mousePosition.x = realMousePosition.x - canvasDestinationRect.x;
        mousePosition.y = realMousePosition.y - canvasDestinationRect.y;
        
        mousePosition.x = (int)(mousePosition.x / (float)(GetScreenWidth() - (canvasDestinationRect.x * 2)) * canvasWidth);
        mousePosition.y = (int)(mousePosition.y / (float)(GetScreenHeight() - (canvasDestinationRect.y * 2)) * canvasHeight);
        
        mousePosition.x = std::clamp(mousePosition.x, 0.0f, (float)canvasWidth - 1);
        mousePosition.y = std::clamp(mousePosition.y, 0.0f, (float)canvasHeight - 1);
        //----------------------------------------------------------------------------------

        // Update
        //----------------------------------------------------------------------------------

        if(IsMouseButtonPressed(0))
        {
            background = GetRandomColor();
            std::cout << TextFormat("(%d,%d)", static_cast<int>(mousePosition.x), static_cast<int>(mousePosition.y)) << "\n";
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
            BeginTextureMode(canvas);
                // clear canvas to color
                ClearBackground(background);
                
                // draw yellow border
                DrawRectangleLinesEx({0.5f, -0.5f, canvasWidth, canvasHeight + 0.5f}, 1.0f, YELLOW);
                
                // draw some text
                DrawTextDropShadow("Hello, raylib", 5, 5, 8, WHITE, BLACK);
                DrawTextDropShadow("Mouse position SHOULD match\nclosely to the circle.\n\nYellow borders should ALWAYS\nbe visible\n\nLEFT MOUSE to change color.", 5, 25, 8, WHITE, BLACK);
                DrawTextDropShadow(TextFormat("(%d,%d)", static_cast<int>(mousePosition.x), static_cast<int>(mousePosition.y)), 5, 220, 8, WHITE, BLACK);
                
                // draw image
                DrawRectangle(5, 115, 16, 16, BLACK);
                DrawTexture(imageFromURL, 5, 115, WHITE);

                // draw the mouse circles
                DrawCircle(mousePosition.x+.5f, mousePosition.y, 3.0f, RED);
                DrawPixel(mousePosition.x+.5f, mousePosition.y+.5f, WHITE);
            EndTextureMode();

            ClearBackground(BLANK);
            DrawTexturePro(canvas.texture, canvasSourceRect, canvasDestinationRect, (Vector2){0, 0}, 0.0f, WHITE);
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(imageFromURL);
    UnloadRenderTexture(canvas);
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
