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

#if defined(__PGETINKER__)
#include "pgetinker.h"
#else
static inline void pgetinker_file_resolve(const char* url, const char* mountPath) {}
#endif

#include <iostream>
#include <stdlib.h>

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
// Global Varialbes
//------------------------------------------------------------------------------------
const int screenWidth = 256;
const int screenHeight = 240;

Texture2D imageFromURL;
Color background;
Vector2 mousePosition;

//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
// Update
//------------------------------------------------------------------------------------
void update()
{
    // Update
    //----------------------------------------------------------------------------------
    mousePosition = GetMousePosition();

    if(IsMouseButtonPressed(0))
    {
        background = GetRandomColor();
        std::cout << TextFormat("(%d,%d)", static_cast<int>(mousePosition.x), static_cast<int>(mousePosition.y)) << "\n";
    }
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();
        // clear canvas to color
        ClearBackground(background);
        
        // draw yellow border
        DrawRectangleLinesEx({0.5f, -0.5f, screenWidth, screenHeight + 0.5f}, 1.0f, YELLOW);
        
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
    EndDrawing();
    
    //----------------------------------------------------------------------------------
}

//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    InitWindow(screenWidth, screenHeight, "raylib - PGEtinker Classic Example");
    
    pgetinker_file_resolve("https://raw.githubusercontent.com/PGEtinker/assets/main/broken.png", "assets/gfx/broken.png");
    imageFromURL = LoadTexture("assets/gfx/broken.png");
    
    background = GetRandomColor();
    mousePosition     = {0, 0};

#if(__EMSCRIPTEN__)
    // Set our game to run at 60 frames-per-second
    emscripten_set_main_loop(update, 60, 1);
#else
    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second

    // Main game loop
    while(!WindowShouldClose())
    {
        update();
    }
#endif
    
    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(imageFromURL);
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
