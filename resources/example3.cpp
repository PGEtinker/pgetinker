#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define OLC_PGEX_MINIAUDIO
#include "olcPGEX_MiniAudio.h"

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

// Override base class with your custom functionality
class Example : public olc::PixelGameEngine
{
public:
    Example()
    {
        // Name your application
        sAppName = "Library Versions Demo";
    }
    
public:
    
    // OnUserCreate is Called once at the start and
    // is where you do things like load files and
    // initilize variables.
    bool OnUserCreate() override
    {
        color = RandomColor();
        
        // PGE version
        versions = "olcPixelGameEngine: v" + std::to_string(PGE_VER / 100) + "." + std::to_string(PGE_VER % 100) + "\n";
        
        // olcPGEX_MiniAudio version
        size_t spacePos = ma.name.find(' ');
        if(spacePos != std::string::npos)
        {
            std::string name = ma.name.substr(0, spacePos);
            std::string version = ma.name.substr(spacePos + 1);
            versions += name + ":  " + version + "\n";
        }
        
        // MiniAudio version
        versions += "MiniAudio:          ";
        versions += MA_VERSION_STRING; versions += "\n";
        
        return true;
    }
    
    // OnUserUpdate is called once per frame and
    // is where you draw things to the screen
    bool OnUserUpdate(float fElapsedTime) override
    {
        // when you left click the mouse
        if(GetMouse(0).bPressed)
        {
            // change the color
            color = RandomColor();
            
            // print out the current mouse position (x, y)
            std::cout << GetMousePos() << "\n";
        }
            
        // clear the screen to the provided color
        Clear(color);

        // Draw the yellow outline
        DrawRect(0,0,ScreenWidth()-1, ScreenHeight()-1, olc::YELLOW);
        
        // draw some test
        DrawStringDropShadow(5,  5, "Library Versions Demo", olc::WHITE, olc::BLACK);
        
        olc::vi2d header = GetTextSize("LIBRARY             VERSION");
        olc::vi2d content = GetTextSize(versions);

        DrawRect(2, 45, content.x + 4, header.y + content.y + 7);
        DrawStringDropShadow(5, 50, "LIBRARY             VERSION", olc::WHITE, olc::BLACK);
        DrawStringDropShadow(5, 65, versions, olc::WHITE, olc::BLACK);

        DrawStringDropShadow(5, 140, "If a library that can be used\nwith PGEtinker is not listed it\nis because there is no runtime\nmeans to determine it's version.", olc::WHITE, olc::BLACK);

        // draw a circle where the mouse is currently located
        FillCircle(GetMousePos(), 3, olc::RED);
        
        // draw a point where the mouse is currently located
        Draw(GetMousePos(), olc::WHITE);
        
        return true;
    }
    
    void DrawStringDropShadow(const int x, const int y, const std::string& text, const olc::Pixel& foregroundColor, const olc::Pixel& backgroundColor)
    {
        DrawString(x + 1, y + 1, text, backgroundColor);
        DrawString(    x,     y, text, foregroundColor);
    }
    
    olc::Pixel RandomColor()
    {
        // we limit to the darker half of the colors
        return olc::Pixel(rand() % 128, rand() % 128, rand() % 128);
    }
    
public: // class variables
    
    olc::Pixel color;
    std::string versions;
    olc::MiniAudio ma;
    
};

int main()
{
    // an instance of the Example, called demo
    Example demo;
    
    // attempt to construct the window/screen 256x240 pixels,
    // with pixels that are 2x2. If successful, start
    // the demo.
    if (demo.Construct(320, 180, 4, 4))
        demo.Start();
    
    // this is the end of the program
    return 0;
}
