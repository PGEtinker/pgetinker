/*
	imgui_impl_pge.h
	+-------------------------------------------------------------+
	|         OneLoneCoder Pixel Game Engine Extension            |
	|               Dear ImGui Integration - v4.0                 |
	+-------------------------------------------------------------+
	What is this?
	~~~~~~~~~~~~~
	This is an extension to the olcPixelGameEngine, which provides
	a method to integrate Dear ImGui in a platform agnostic way.

	Author
	~~~~~~
	Dandistine

	License (OLC-3)
	~~~~~~~~~~~~~~~
	Copyright 2018 - 2019 OneLoneCoder.com
	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions
	are met:
	1. Redistributions or derivations of source code must retain the above
	copyright notice, this list of conditions and the following disclaimer.
	2. Redistributions or derivative works in binary form must reproduce
	the above copyright notice. This list of conditions and the following
	disclaimer must be reproduced in the documentation and/or other
	materials provided with the distribution.
	3. Neither the name of the copyright holder nor the names of its
	contributors may be used to endorse or promote products derived
	from this software without specific prior written permission.
	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
	A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
	HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


/*
Versions:
1.0 - Initial release
2.0 - Addition of OpenGl 3.3 rendering option - PGE 2.11+ required
	- ImGui related OpenGL functions pulled into extension and no longer need
	  to be called by the application.
		ImGui::CreateContext
		ImGui::NewFrame
		ImGui_ImplOpenGL2_Init
		ImGui_ImplOpenGL2_NewFrame
		ImGui_ImplOpenGL2_RenderDrawData
		ImGui_ImplOpenGL3_Init
		ImGui_ImplOpenGL3_NewFrame
		ImGui_ImplOpenGL3_RenderDrawData
	- Keymap is now matches PGE and includes more symbols like []{}
	- Add support for Before/After OnUserCreate/OnUserUpdate being run automatically by PGE
	- Extension automatically registers with PGE using the new 2.10+ system
3.0 - OnBeforeUserUpdate now returns false to work with PGE 2.17
4.0 - Update to support new Dear Imgui input method.  Dear ImGui 1.91.5 now required
*/


/*

KNOWN LIMITATIONS
PGE does not bind every key on the keyboard.
	- ALT is not bound
	- No distinction between L-CTRL and R-CTRL
	- Maybe no distinction between ENTER and NP_ENTER
		There is no NP_ENTER, but there is both an ENTER and a RETURN
	- The Super / Windows key is not bound

There is no IME support for non-English languages.

There is no support for non-US or non-QWERTY keyboards although they may work.

PGE returns mouse scrolling using an integer, and the relation of this integer
to actual scroll distance is not well understood by me.  On a Logitech G700s a
single "click" of the wheel is a wheel delta of +/-120.  The scroll sensitivity
defaults to 120.0f and can be changed with ImGui_ImplPGE_SetScrollSensitivity.
Higher values result in less scrolling per movement of the wheel.

Dear ImGui will always render 1:1 with the display's pixels.  You cannot have
Dear ImGui render its UI using pixels with size matching the application.


Dear ImGui Integration Implementation

Include this file in your application and define the implementation macro
in at least one location.  The macro will create the implementation of the
extension's functions.  It must be defined before the include in the file
where the implementation is needed.  A separate .cpp file is recommended
for this purpose.  A PGE version of at least 2.06 is required for some of
the functions used.  This can be identified in the PGE header file.  A PGE
version of at least 2.10 is required to use the OpenGL 3.3 renderer.  The
OpenGL 3.3 renderer may be specified by defining PGE_GFX_OPENGL33  in the
same location as OLC_PGEX_DEAR_IMGUI_IMPLEMENTATION.

```
//OpenGL 2 renderer
#define OLC_PGEX_DEAR_IMGUI_IMPLEMENTATION
#include imgui_impl_pge.h
```

```
//OpenGL 3.3 renderer
#define PGE_GFX_OPENGL33
#define OLC_PGEX_DEAR_IMGUI_IMPLEMENTATION
#include imgui_impl_pge.h
```

Dear ImGui Integration Usage

Unlike many PGE Extensions, this extension requires files from an external
library.  To integrate the Dear Imgui functionality you will need to pull
the Dear ImGui software and include a number of files in your project.

Dear ImGui Github:
https://github.com/ocornut/imgui

Version Required:
v1.91.5

Files Required:
imgui.h
imgui_internal.h
imstb_rectpack.h
imstb_textedit.h
imstb_truetype.h

imgui.cpp
imgui_demo.cpp
imgui_draw.cpp
imgui_widgets.cpp
Imgui_tables.cpp

Additionally required are either the opengl2 or opengl3 files, depending
on the choice of renderer

imgui_impl_opengl2.cpp
imgui_impl_opengl2.h

imgui_impl_opengl3.cpp
imgui_impl_opengl3.h
imgui_impl_opengl3_loader.h



Once all these files are included in your project you will be able to use
Dear ImGui.  Documentation on using the Dear ImGui library can be found
on their Github.

To integrate Dear ImGui with a PGE application, there are some special
things to consider.  First, Dear ImGui will render to the screen directly
using OpenGL and the GPU.  PGE has one facility to support this rendering
method, and that is the Layer's funchook which is intended to allow a game
to render a layer using its own renderer code.  This is how we will render
the Dear ImGui interface.  Second, we want the UI to appear on top of the
rest of the game.  Layer 0 is the top-most layer, so we will need to use
the Layer 0 render hook, and do all other PGE drawing to Layer 1 (or more).

Instead of writing paragraph upon paragraph on how to do this, I will just
add a basic example below:

class Example : public olc::PixelGameEngine
{
	olc::imgui::PGE_ImGUI pge_imgui;
	int m_GameLayer;

public:
	//PGE_ImGui can automatically call the SetLayerCustomRenderFunction by passing
	//true into the constructor.  false is the default value.
	Example() : pge_imgui(false)
	{
		sAppName = "Test Application";
	}

public:
	bool OnUserCreate() override
	{
		//Create a new Layer which will be used for the game
		m_GameLayer = CreateLayer();
		//The layer is not enabled by default,  so we need to enable it
		EnableLayer(m_GameLayer, true);

		//Set a custom render function on layer 0.  Since DrawUI is a member of
		//our class, we need to use std::bind
		//If the pge_imgui was constructed with _register_handler = true, this line is not needed
		SetLayerCustomRenderFunction(0, std::bind(&Example::DrawUI, this));

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		//Change the Draw Target to not be Layer 0
		SetDrawTarget((uint8_t)m_GameLayer);
		//Game Drawing code here

		//Create and react to your UI here, it will be drawn during the layer draw function
		ImGui::ShowDemoWindow();

		return true;
	}

	void DrawUI(void) {
		//This finishes the Dear ImGui and renders it to the screen
		pge_imgui.ImGui_ImplPGE_Render();
	}
};

int main() {
	Example demo;
	if (demo.Construct(640, 360, 2, 2))
		demo.Start();

	return 0;
}

*/

#pragma once
#ifndef OLC_PGEX_IMGUI_IMPL_PGE_H
#define OLC_PGEX_IMGUI_IMPL_PGE_H

#include <vector>
#include "imgui.h"
#include "olcPixelGameEngine.h"



namespace olc
{
	//Avoid polluting the olc namespace as much as we can
	namespace imgui
	{
		struct keyCharMap {
			olc::Key key;
			ImGuiKey imgui_key;
			char lower;
			char upper;
		};

		class PGE_ImGUI : public PGEX
		{
		public:

			//_register_handler set to true will automatically register this plugin to draw on layer 0
			PGE_ImGUI(bool _register_handler = false);

			//Initialize the Dear ImGui PGE Platform implementation.
			olc::rcode ImGui_ImplPGE_Init();

			//Shutdown and perform Dear ImGui Platform cleanup
			void ImGui_ImplPGE_Shutdown(void);

			//Begin a new frame.  Must be called AFTER the renderer new frame
			void ImGui_ImplPGE_NewFrame(void);

			//Adjust the sensitivity of the mouse scroll wheel.  Default value is 120.0f
			void ImGui_ImplPGE_SetScrollSensitivity(float val);

			//Renders the UI to the screen.  Calls ImGui::Render() and ImGui_ImplOpenGL[23]_RenderDrawData() internally
			//Depending on the configured renderer.
			void ImGui_ImplPGE_Render(void);

			//A group of PGEX functions which will be automatically run by PGE at specific times
			void OnBeforeUserCreate() override;
			void OnAfterUserCreate() override;
			bool OnBeforeUserUpdate(float& fElapsedTime) override;
			void OnAfterUserUpdate(float fElapsedTime) override;

		private:
			//Mapping of olc::Key to ImGuiKey and upper / lowercase inputs
			std::vector<keyCharMap> vKeys;

			//Larger number = less scrolling.  Negative inverts scroll direction
			float fScrollSensitivity = 120.0f;

			//A flag to determine if the Extensions should automatically create a layer and add the
			//UI draw handler as the render function for that layer
			bool register_handler = false;

			//Internal function to update Dear ImGui with the current state of the mouse
			void ImGui_ImplPGE_UpdateMouse(void);

			//Internal function to update Dear ImGui with the current state of the keyboard
			void ImGui_ImplPGE_UpdateKeys(void);
		};

		// Allow Dear ImGui to change the blend mode
		void PGE_ImGUI_BlendModeCallback(const ImDrawList* parent_list, const ImDrawCmd* cmd);
	}

}

#ifdef OLC_PGEX_DEAR_IMGUI_IMPLEMENTATION
#ifdef OLC_GFX_OPENGL33
#include "imgui_impl_opengl3.h"
#else
#include "imgui_impl_opengl2.h"
#endif

namespace olc
{
	namespace imgui
	{
		PGE_ImGUI::PGE_ImGUI(bool _register_handler) : PGEX(true), register_handler(_register_handler) {

		}

		olc::rcode PGE_ImGUI::ImGui_ImplPGE_Init() {
			ImGui::CreateContext();

#ifdef OLC_GFX_OPENGL33
			ImGui_ImplOpenGL3_Init();
#else
			ImGui_ImplOpenGL2_Init();
#endif
			ImGuiIO& io = ImGui::GetIO();

			io.BackendPlatformName = "imgui_impl_pge_4.0";

			//Map keys which input values to input boxes.
			//This is not the ideal way to do this, but PGE does not expose
			//much in the way of keyboard or IME input.  Many special characters
			//are absent as well.
			vKeys = {
				{olc::A, ImGuiKey_A, 'a', 'A'},
				{olc::B, ImGuiKey_B, 'b', 'B'},
				{olc::C, ImGuiKey_C, 'c', 'C'},
				{olc::D, ImGuiKey_D, 'd', 'D'},
				{olc::E, ImGuiKey_E, 'e', 'E'},
				{olc::F, ImGuiKey_F, 'f', 'F'},
				{olc::G, ImGuiKey_G, 'g', 'G'},
				{olc::H, ImGuiKey_H, 'h', 'H'},
				{olc::I, ImGuiKey_I, 'i', 'I'},
				{olc::J, ImGuiKey_J, 'j', 'J'},
				{olc::K, ImGuiKey_K, 'k', 'K'},
				{olc::L, ImGuiKey_L, 'l', 'L'},
				{olc::M, ImGuiKey_M, 'm', 'M'},
				{olc::N, ImGuiKey_N, 'n', 'N'},
				{olc::O, ImGuiKey_O, 'o', 'O'},
				{olc::P, ImGuiKey_P, 'p', 'P'},
				{olc::Q, ImGuiKey_Q, 'q', 'Q'},
				{olc::R, ImGuiKey_R, 'r', 'R'},
				{olc::S, ImGuiKey_S, 's', 'S'},
				{olc::T, ImGuiKey_T, 't', 'T'},
				{olc::U, ImGuiKey_U, 'u', 'U'},
				{olc::V, ImGuiKey_V, 'v', 'V'},
				{olc::W, ImGuiKey_W, 'w', 'W'},
				{olc::X, ImGuiKey_X, 'x', 'X'},
				{olc::Y, ImGuiKey_Y, 'y', 'Y'},
				{olc::Z, ImGuiKey_Z, 'z', 'Z'},
				{olc::K0, ImGuiKey_0, '0', ')'},
				{olc::K1, ImGuiKey_1, '1', '!'},
				{olc::K2, ImGuiKey_2, '2', '@'},
				{olc::K3, ImGuiKey_3, '3', '#'},
				{olc::K4, ImGuiKey_4, '4', '$'},
				{olc::K5, ImGuiKey_5, '5', '%'},
				{olc::K6, ImGuiKey_6, '6', '^'},
				{olc::K7, ImGuiKey_7, '7', '&'},
				{olc::K8, ImGuiKey_8, '8', '*'},
				{olc::K9, ImGuiKey_9, '9', '('},
				{olc::NP0, ImGuiKey_Keypad0, '0', '0'},
				{olc::NP1, ImGuiKey_Keypad1, '1', '1'},
				{olc::NP2, ImGuiKey_Keypad2, '2', '2'},
				{olc::NP3, ImGuiKey_Keypad3, '3', '3'},
				{olc::NP4, ImGuiKey_Keypad4, '4', '4'},
				{olc::NP5, ImGuiKey_Keypad5, '5', '5'},
				{olc::NP6, ImGuiKey_Keypad6, '6', '6'},
				{olc::NP7, ImGuiKey_Keypad7, '7', '7'},
				{olc::NP8, ImGuiKey_Keypad8, '8', '8'},
				{olc::NP9, ImGuiKey_Keypad9, '9', '9'},
				{olc::NP_MUL, ImGuiKey_KeypadMultiply, '*', '*'},
				{olc::NP_DIV, ImGuiKey_KeypadDivide, '/', '/'},
				{olc::NP_ADD, ImGuiKey_KeypadAdd, '+', '+'},
				{olc::NP_SUB, ImGuiKey_KeypadSubtract, '-', '-'},
				{olc::NP_DECIMAL, ImGuiKey_KeypadDecimal, '.', '.'},
				{olc::PERIOD, ImGuiKey_Period, '.', '>'},
				{olc::SPACE, ImGuiKey_Space, ' ', ' '},
				{olc::OEM_1, ImGuiKey_Semicolon, ';', ':'},
				{olc::OEM_2, ImGuiKey_Slash, '/', '?'},
				{olc::OEM_3, ImGuiKey_GraveAccent, '`', '~'},
				{olc::OEM_4, ImGuiKey_LeftBracket, '[', '{'},
				{olc::OEM_5, ImGuiKey_Backslash, '\\', '|'},
				{olc::OEM_6, ImGuiKey_RightBracket, ']', '}'},
				{olc::OEM_7, ImGuiKey_Apostrophe, '\'', '"'},
				{olc::OEM_8, ImGuiKey_Minus, '-', '-'},
				{olc::EQUALS, ImGuiKey_Equal, '=', '+'},
				{olc::COMMA, ImGuiKey_Comma, ',', '<'},
				{olc::MINUS, ImGuiKey_Minus, '-', '_'},
				{olc::TAB, ImGuiKey_Tab, '\t', '\t'},

				// These keys do not trigger any character input
				{olc::LEFT, ImGuiKey_LeftArrow, '\0', '\0'},
				{olc::RIGHT, ImGuiKey_RightArrow, '\0', '\0'},
				{olc::UP, ImGuiKey_UpArrow, '\0', '\0'},
				{olc::DOWN, ImGuiKey_DownArrow, '\0', '\0'},
				{olc::PGUP, ImGuiKey_PageUp, '\0', '\0'},
				{olc::PGDN, ImGuiKey_PageDown, '\0', '\0'},
				{olc::HOME, ImGuiKey_Home, '\0', '\0'},
				{olc::END, ImGuiKey_End, '\0', '\0'},
				{olc::INS, ImGuiKey_Insert, '\0', '\0'},
				{olc::DEL, ImGuiKey_Delete, '\0', '\0'},
				{olc::BACK, ImGuiKey_Backspace, '\0', '\0'},
				{olc::ENTER, ImGuiKey_Enter, '\0', '\0'},
				{olc::ESCAPE, ImGuiKey_Escape, '\0', '\0'},
			};

			return olc::OK;
		}

		//This currently does nothing, but is defined in the event that it needs to eventually do something
		void PGE_ImGUI::ImGui_ImplPGE_Shutdown(void) {
			return;
		}

		void PGE_ImGUI::ImGui_ImplPGE_UpdateMouse(void) {
			ImGuiIO& io = ImGui::GetIO();
			olc::vi2d windowMouse = pge->GetWindowMouse();

			io.AddMouseSourceEvent(ImGuiMouseSource::ImGuiMouseSource_Mouse);

			//Update Mouse Buttons
			for (int i = 0; i < olc::nMouseButtons; i++) {
				if (pge->GetMouse(i).bPressed) {
					io.AddMouseButtonEvent(i, true);
				}
				else if (pge->GetMouse(i).bReleased) {
					io.AddMouseButtonEvent(i, false);
				}
			}
			
			//Update the Mouse Position
			io.AddMousePosEvent((float)windowMouse.x, (float)windowMouse.y);

			//Update Mouse Wheel
			io.AddMouseWheelEvent(0.0f, static_cast<float>(pge->GetMouseWheel()) / fScrollSensitivity);
		}

		void PGE_ImGUI::ImGui_ImplPGE_UpdateKeys(void) {
			ImGuiIO& io = ImGui::GetIO();

			io.AddKeyEvent(ImGuiMod_Ctrl, pge->GetKey(olc::CTRL).bHeld);
			io.AddKeyEvent(ImGuiMod_Shift, pge->GetKey(olc::SHIFT).bHeld);
			io.AddKeyEvent(ImGuiMod_Alt, false);
			io.AddKeyEvent(ImGuiMod_Super, false);

			//Get the status of the SHIFT key to so we can change the inserted character
			bool isShift = pge->GetKey(SHIFT).bHeld;

			//Iterate the key list an send the appropriate press / release events to Dear ImGui
			for (auto& m : vKeys) {
				if (pge->GetKey(m.key).bPressed) {
					io.AddKeyEvent(m.imgui_key, true);

					//If the key has an associated character then send that too
					if (m.lower != '\0') {
						io.AddInputCharacter(isShift ? m.upper : m.lower);
					}
				}
				else if (pge->GetKey(m.key).bReleased) {
					io.AddKeyEvent(m.imgui_key, false);
				}
			}
		}

		void PGE_ImGUI::ImGui_ImplPGE_NewFrame(void) {
#ifdef OLC_GFX_OPENGL33
			ImGui_ImplOpenGL3_NewFrame();
#else
			ImGui_ImplOpenGL2_NewFrame();
#endif
			ImGuiIO& io = ImGui::GetIO();
			olc::vi2d windowSize = pge->GetWindowSize();
			IM_ASSERT(io.Fonts->IsBuilt() && "Font atlas not built! It is generally built by the renderer back-end. Missing call to renderer _NewFrame() function? e.g. ImGui_ImplOpenGL2_NewFrame().");

			io.DisplaySize = ImVec2((float)windowSize.x, (float)windowSize.y);

			io.DeltaTime = pge->GetElapsedTime();

			ImGui_ImplPGE_UpdateKeys();

			ImGui_ImplPGE_UpdateMouse();
		}

		void PGE_ImGUI::ImGui_ImplPGE_SetScrollSensitivity(float val) {
			this->fScrollSensitivity = val;
		}

		void PGE_ImGUI::ImGui_ImplPGE_Render(void) {
			//This finishes the Dear ImGui and renders it to the screen
			ImGui::Render();
#ifdef OLC_GFX_OPENGL33
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#else
			ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
#endif
		}

		//Before the OnUserCreate function runs, we will register the UI drawing function
		//to the layer 0, if we were told to do so during construction
		//We can't register this during construction because the pge-vLayers is not configured until after construction
		void PGE_ImGUI::OnBeforeUserCreate() {
			if (register_handler) {
				pge->SetLayerCustomRenderFunction(0, std::bind(&PGE_ImGUI::ImGui_ImplPGE_Render, this));
			}
		}

		//After the OnUserCreate function runs, we will run all of the Init code to setup
		//the ImGui.  This will happen automatically in PGE 2.10+
		void PGE_ImGUI::OnAfterUserCreate() {
			ImGui_ImplPGE_Init();
		}

		//Before the OnUserUpdate runs, do the pre-frame ImGui intialization
		bool PGE_ImGUI::OnBeforeUserUpdate(float& fElapsedTime) {
			ImGui_ImplPGE_NewFrame();
			ImGui::NewFrame();
			return false;
		}

		//There is currently no "after update" logic to run for ImGui
		void PGE_ImGUI::OnAfterUserUpdate(float fElapsedTime) {}

		//Change the GL blending function.  Useful when overlaying images with the Dear ImGui Image function
		//As this does not automatically change the mode back to normal, a second call to reset the mode will
		//generally be required
		void PGE_ImGUI_BlendModeCallback(const ImDrawList* parent_list, const ImDrawCmd* cmd) {
			const olc::DecalMode mode = (olc::DecalMode)reinterpret_cast<std::intptr_t>(cmd->UserCallbackData);
			switch (mode)
			{
			case olc::DecalMode::NORMAL:
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				break;
			case olc::DecalMode::ADDITIVE:
				glBlendFunc(GL_SRC_ALPHA, GL_ONE);
				break;
			case olc::DecalMode::MULTIPLICATIVE:
				glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);
				break;
			case olc::DecalMode::STENCIL:
				glBlendFunc(GL_ZERO, GL_SRC_ALPHA);
				break;
			case olc::DecalMode::ILLUMINATE:
				glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);
				break;
			case olc::DecalMode::WIREFRAME:
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				break;
			}
		}
	}
}
#endif //OLC_PGEX_DEAR_IMGUI_IMPLEMENTATION
#endif //OLC_PGE_IMGUI_IMPL_PGE_H