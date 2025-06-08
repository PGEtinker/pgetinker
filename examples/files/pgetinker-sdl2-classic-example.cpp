/*******************************************************************************************
*
*   SDL2 example - PGEtinker Classic Example
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
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <pgetinker.h>

//------------------------------------------------------------------------------------
// wrap up SDL_texture loading 
//------------------------------------------------------------------------------------
struct Sprite
{
    Sprite(SDL_Renderer* renderer, const std::string& path)
        : renderer(renderer)
    {
        LoadFromFile(path);
    }
    
    ~Sprite()
    {
        SDL_DestroyTexture(texture);
    }

public:
    bool LoadFromFile(const std::string& path)
    {
        SDL_Surface* surface = IMG_Load(path.c_str());
        if(!surface)
        {
            status = Status::SURFACE_FAIL;
            return false;
        }
        
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        if(!texture)
        {
            status = Status::TEXTURE_FAIL;
            return false;
        }
        
        width  = surface->w;
        height = surface->h;

        SDL_FreeSurface(surface);

        status = Status::READY;
        return true;
    }
    
    std::string getError()
    {
        std::string retval{IMG_GetError()};
        return retval;
    }
    
    enum class Status : int
    {
        READY = 0,
        NOT_READY,
        SURFACE_FAIL,
        TEXTURE_FAIL
    };
    
    Status status = Status::NOT_READY;
    int width = 0;
    int height = 0;
    
    operator SDL_Texture*() const
    {
        return texture;
    }

private:
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* texture = nullptr;

};
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
// Simple color struct to mimic olc::Pixel
//------------------------------------------------------------------------------------
struct Color {
    Uint8 r, g, b, a;
    Color(Uint8 r_ = 0, Uint8 g_ = 0, Uint8 b_ = 0, Uint8 a_ = 255) : r(r_), g(g_), b(b_), a(a_) {}
};

//------------------------------------------------------------------------------------
// Application State
//------------------------------------------------------------------------------------
struct ApplicationState
{
    SDL_Window* window;
    SDL_Renderer* renderer;

    int screenWidth;
    int screenHeight;
    int screenScale;
    bool running;

    bool prevLeftMouse;
    bool currentLeftMouse;

    Color backgroundColor;
    Sprite* sprite;
    TTF_Font* font;
};

ApplicationState app = {0};
//------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
// Draw Text with a drop shadow of the provided foreground/background colors.
//------------------------------------------------------------------------------------
void DrawStringDropShadow(SDL_Renderer* renderer, TTF_Font* font, int x, int y, const std::string& text, Color fg, Color bg) {
    SDL_Color sdl_fg = {fg.r, fg.g, fg.b, fg.a};
    SDL_Color sdl_bg = {bg.r, bg.b, bg.b, bg.a};

    // Render background (shadow)
    SDL_Surface* bg_surface = TTF_RenderText_Blended_Wrapped(font, text.c_str(), sdl_bg, (app.screenWidth * app.screenScale) - x);
    SDL_Texture* bg_texture = SDL_CreateTextureFromSurface(renderer, bg_surface);
    SDL_Rect bg_rect = {x + (1 * app.screenScale), y + (1 * app.screenScale), bg_surface->w, bg_surface->h};
    SDL_RenderCopy(renderer, bg_texture, nullptr, &bg_rect);
    SDL_FreeSurface(bg_surface);
    SDL_DestroyTexture(bg_texture);

    // Render foreground
    SDL_Surface* fg_surface = TTF_RenderText_Solid_Wrapped(font, text.c_str(), sdl_fg, (app.screenWidth * app.screenScale) - x);
    SDL_Texture* fg_texture = SDL_CreateTextureFromSurface(renderer, fg_surface);
    SDL_Rect fg_rect = {x, y, fg_surface->w, fg_surface->h};
    SDL_RenderCopy(renderer, fg_texture, nullptr, &fg_rect);
    SDL_FreeSurface(fg_surface);
    SDL_DestroyTexture(fg_texture);
}

//------------------------------------------------------------------------------------
// Draw a filled circle centered on the provided coordinates.
//------------------------------------------------------------------------------------
void FillCircle(SDL_Renderer* renderer, int cx, int cy, int radius, Color color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    for (int y = -radius; y <= radius; y++)
    {
        for(int x = -radius; x <= radius; x++)
        {
            if(x * x + y * y <= radius * radius)
            {
                SDL_RenderDrawPoint(renderer, cx + x, cy + y);
            }
        }
    }
}

//------------------------------------------------------------------------------------
// Get a randomized color of full alpha.
//------------------------------------------------------------------------------------
Color GetRandomColor()
{
    return Color(rand() % 128, rand() % 128, rand() % 128);
}

//------------------------------------------------------------------------------------
// Update
//------------------------------------------------------------------------------------
void update()
{
    // Handle events
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            app.running = false;
        }
    }

    // Get mouse state
    int mouse_x, mouse_y;
    Uint32 mouse_state = SDL_GetMouseState(&mouse_x, &mouse_y);
    app.currentLeftMouse = (mouse_state & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;

    // Detect mouse press (transition from not pressed to pressed)
    if (app.currentLeftMouse && !app.prevLeftMouse) {
        app.backgroundColor = GetRandomColor();
        std::cout << "Mouse Pos: (" << mouse_x << ", " << mouse_y << ")\n";
    }
    app.prevLeftMouse = app.currentLeftMouse;


    // Clear screen with background color
    SDL_SetRenderDrawColor(app.renderer, app.backgroundColor.r, app.backgroundColor.g, app.backgroundColor.b, app.backgroundColor.a);
    SDL_RenderClear(app.renderer);

    // Draw yellow border
    SDL_SetRenderDrawColor(app.renderer, 255, 255, 0, 255); // Yellow
    SDL_Rect border = {0, 0, app.screenWidth*app.screenScale - 1, app.screenHeight*app.screenScale - 1};
    SDL_RenderDrawRect(app.renderer, &border);

    // Draw text
    DrawStringDropShadow(app.renderer, app.font, 5*app.screenScale, 5*app.screenScale, "Hello, SDL2", Color(255, 255, 255), Color(0, 0, 0));
    DrawStringDropShadow(app.renderer, app.font, 5*app.screenScale, 25*app.screenScale,
                            "Mouse position SHOULD match\nclosely to the circle.\n\nYellow borders should ALWAYS\nbe visible\n\nLEFT MOUSE to change color.",
                            Color(255, 255, 255), Color(0, 0, 0));
    DrawStringDropShadow(app.renderer, app.font, 5*app.screenScale, 220*app.screenScale, "Mouse Pos: (" + std::to_string(mouse_x) + ", " + std::to_string(mouse_y) + ")",
                            Color(255, 255, 255), Color(0, 0, 0));

    // Draw image
    SDL_Rect image_rect = {
            5 * app.screenScale,
        110 * app.screenScale,
        app.sprite->width * app.screenScale,
        app.sprite->height * app.screenScale
    };
    SDL_SetRenderDrawColor(app.renderer, 0,0,0,255);
    SDL_RenderFillRect(app.renderer, &image_rect);
    SDL_RenderCopy(app.renderer, *app.sprite, nullptr, &image_rect);

    // Draw mouse cursor (circle and point)
    FillCircle(app.renderer, mouse_x, mouse_y, 3*app.screenScale, Color(255, 0, 0)); // Red circle
    SDL_SetRenderDrawColor(app.renderer, 255, 255, 255, 255); // White
    SDL_RenderDrawPoint(app.renderer, mouse_x, mouse_y);

    // Present renderer
    SDL_RenderPresent(app.renderer);
}
//------------------------------------------------------------------------------------


int main()
{
    app.screenWidth = 256;
    app.screenHeight = 240;
    app.screenScale = 2;
    
    // Initialize SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Initialize SDL_image
    if(IMG_Init(IMG_INIT_PNG) == 0)
    {
        std::cerr << "IMG_Init failed: " << IMG_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Initialize SDL_ttf
    if(TTF_Init() == -1)
    {
        std::cerr << "TTF_Init failed: " << TTF_GetError() << std::endl;
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Create window
    app.window = SDL_CreateWindow(
        "PGEtinker Classic Example (SDL2)",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        app.screenWidth * app.screenScale, app.screenHeight * app.screenScale,
        0
    );
    if(!app.window)
    {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Create renderer
    app.renderer = SDL_CreateRenderer(app.window, -1, SDL_RENDERER_ACCELERATED);
    if(!app.renderer)
    {
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(app.window);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Load font (adjust path to a TrueType font available on your system)
    pgetinker_file_resolve("https://raw.githubusercontent.com/qwerasd205/PixelCode/main/dist/ttf/PixelCode.ttf", "assets/gfx/font.ttf");
    app.font = TTF_OpenFont("assets/gfx/font.ttf", 8 * app.screenScale); // Replace with a valid font path
    if(!app.font)
    {
        std::cerr << "TTF_OpenFont failed: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(app.renderer);
        SDL_DestroyWindow(app.window);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Load image
    pgetinker_file_resolve("https://raw.githubusercontent.com/PGEtinker/assets/main/broken.png", "assets/gfx/broken.png");
    app.sprite = new Sprite(app.renderer, "assets/gfx/broken.png");
    if(app.sprite->status != Sprite::Status::READY)
    {
        std::cerr << app.sprite->getError() << std::endl;
        TTF_CloseFont(app.font);
        SDL_DestroyRenderer(app.renderer);
        SDL_DestroyWindow(app.window);
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    app.backgroundColor = GetRandomColor();
    app.running = true;

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(update, 0, 1);
#else
    while (app.running)
    {
        update();
    }
#endif

    // Cleanup
    delete app.sprite;

    TTF_CloseFont(app.font);
    SDL_DestroyRenderer(app.renderer);
    SDL_DestroyWindow(app.window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}