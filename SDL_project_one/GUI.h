#ifndef GUI_H
#define GUI_H

#include "common.h"
#include "Input.h"

namespace GUI
{
	struct GUIText
	{
		SDL_Texture* texture;
		vec2 pos;
		vec2 dims;
	};

	struct GUIButton
	{
		SDL_Rect border;
		GUIText text;
		bool hovering;
		void (*OnClicked)(void);
	};

	struct GUIWindow
	{
		bool		isActive;
		SDL_Rect	titleBar;
		SDL_Rect	border;
		GUIText		title;
		GUIText		text;
		GUIButton	exitButton;
	};

	void CreateGUIWindow(GUIWindow* window, int x, int y, SDL_Texture* title, SDL_Texture* text );
	void CreateGUIButton(GUIButton* button, int x, int y, SDL_Texture* text, void (*OnClick)(void) = 0);

	void DrawGUIWindow(SDL_Renderer* renderer, GUIWindow* window);
	void DrawGUIButton(SDL_Renderer* renderer, GUIButton* button);
	void GUIUpdateButton( GUIButton* button, Input::MouseInput *mouse);
	void GUIUpdateWindow( GUIWindow* window, Input::MouseInput *mouse);
	void DrawGUIText(SDL_Renderer* renderer, GUIText* gui);

};

#endif