/*******************************************************************************************
*
*   olcPixelGameEngine example - PGEtinker Classic Example
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
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "pgetinker.h"

class PGEtinkerClassicExample : public olc::PixelGameEngine
{
public:
    PGEtinkerClassicExample()
    {
        sAppName = "PGEtinker Classic Example";
    }
    
    bool OnUserCreate() override
    {
        pgetinker_file_resolve("https://raw.githubusercontent.com/PGEtinker/assets/main/broken.png", "assets/gfx/broken.png");
        image.Load("assets/gfx/broken.png");

        color = RandomColor();
        return true;
    }
    
    bool OnUserUpdate(float fElapsedTime) override
    {
        // if left mouse is pressed, get a new color and print the mouse coords
        if(GetMouse(0).bPressed)
        {
            color = RandomColor();
            std::cout << GetMousePos() << "\n";
        }
            
        Clear(color);
        DrawRect(0,0,ScreenWidth()-1, ScreenHeight()-1, olc::YELLOW);
        
        // draw some text
        DrawStringDropShadow(5,  5, "Hello, olcPixelGameEngine", olc::WHITE, olc::BLACK);
        DrawStringDropShadow(5, 25, "Mouse position SHOULD match\nclosely to the circle.\n\nYellow borders should ALWAYS\nbe visible\n\nLEFT MOUSE to change color.", olc::WHITE, olc::BLACK);
        DrawStringDropShadow(5, 220, GetMousePos().str(), olc::WHITE, olc::BLACK);
        
        // draw the image
        DrawSprite(5, 100, image.Sprite());
        
        // draw the mouse
        FillCircle(GetMousePos(), 3, olc::RED);
        Draw(GetMousePos(), olc::WHITE);
        
        return true;
    }
    
    // draw a string with a dropshadow of the provided foreground/background colors
    void DrawStringDropShadow(const int x, const int y, const std::string& text, const olc::Pixel& foregroundColor, const olc::Pixel& backgroundColor)
    {
        DrawString(x + 1, y + 1, text, backgroundColor);
        DrawString(    x,     y, text, foregroundColor);
    }
    
    // get a random color
    olc::Pixel RandomColor()
    {
        return olc::Pixel(rand() % 128, rand() % 128, rand() % 128);
    }
    
private:
    olc::Pixel color;
    olc::Renderable image;
};

int main()
{
    PGEtinkerClassicExample example;
    
    if (example.Construct(256, 240, 2, 2))
        example.Start();
    
    return 0;
}