/*******************************************************************************************
*
*   olcPixelGameEngine example - Dear Imgui Example
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
#define PGE_GFX_OPENGL33
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define OLC_PGEX_DEAR_IMGUI_IMPLEMENTATION
#include "imgui_impl_pge.h"

class PGEtinkerDearImguiExample : public olc::PixelGameEngine
{
public:
	PGEtinkerDearImguiExample() : pge_imgui(true)
	{
		sAppName = "PGEtinker Dear Imgui Example";
	}

public:
	bool OnUserCreate() override
	{
		// create and enable a new layer for rendering the game
		gameLayer = CreateLayer();
		EnableLayer(gameLayer, true);
		
        return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// change to the game layer
		SetDrawTarget(gameLayer);
		
        // do drawing here

        Clear(olc::Pixel(0, 33, 98));
        DrawRect(0, 0, ScreenWidth()-1, ScreenHeight()-1, olc::YELLOW);

        // imgui stuff goes here
        // https://github.com/ocornut/imgui/wiki
		ImGui::ShowDemoWindow();

		return true;
	}

private:
	olc::imgui::PGE_ImGUI pge_imgui;
	int gameLayer;
};

int main()
{
	PGEtinkerDearImguiExample demo;
	if (demo.Construct(640, 360, 2, 2))
		demo.Start();

	return 0;
}