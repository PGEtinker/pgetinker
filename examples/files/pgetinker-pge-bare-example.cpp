/*******************************************************************************************
*
*   olcPixelGameEngine example - PGEtinker Bare Example
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

#if defined(__PGETINKER__)
#include "pgetinker.h"
#else
static inline void pgetinker_file_resolve(const char* url, const char* mountPath) {}
#endif

// Override base class with your custom functionality
class PGEtinkerBareExample : public olc::PixelGameEngine
{
public:
    PGEtinkerBareExample()
    {
        // Name your application
        sAppName = "PGEtinker Bare PGE Example";
    }
    
public:
    bool OnUserCreate() override
    {
        // Called once at the start, so create things here
        return true;
    }
    
    bool OnUserUpdate(float fElapsedTime) override
    {
        // Called once per frame, handle input and draw things here
        Clear(olc::BLACK);
        DrawString(
            olc::vi2d{GetScreenSize() / 2} - olc::vi2d{GetTextSize("Bare Example") / 2},
            "Bare Example",
            olc::WHITE
        );
        
        return true;
    }
};

int main()
{
    PGEtinkerBareExample demo;
    if (demo.Construct(256, 240, 2, 2))
        demo.Start();
    return 0;
}
