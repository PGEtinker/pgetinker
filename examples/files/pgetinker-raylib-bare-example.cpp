/*******************************************************************************************
*
*   raylib example - PGEtinker Bare Example
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

//------------------------------------------------------------------------------------
// Global Varialbes
//------------------------------------------------------------------------------------
const int screenWidth  = 256;
const int screenHeight = 240;
const int fontSize     = 8;

//------------------------------------------------------------------------------------
// Update
//------------------------------------------------------------------------------------
void update()
{
    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();
    
        ClearBackground(BLACK);
        DrawText(
            "Bare Example",
            screenWidth / 2 - (MeasureText("Bare Example", fontSize) / 2),
            screenHeight / 2 - fontSize / 2,
            fontSize,
            WHITE
        );
    
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
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
