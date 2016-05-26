#include "GUI.h"


void GUI::DrawGUIButton(SDL_Renderer* renderer, GUIButton* button)
{
	if(button->text.texture)
	{
		if(button->hovering)
		{
			SDL_SetRenderDrawColor(renderer, 50, 50, 255, 200);
		}
		else
		{
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 200);
		}

		SDL_RenderFillRect(renderer, &button->border);
		
		SDL_Rect txtRct;
		txtRct.x = button->text.pos.x;
		txtRct.y = button->text.pos.y;
		txtRct.w = button->text.dims.x;
		txtRct.h = button->text.dims.y;

		SDL_RenderCopy(renderer, button->text.texture, 0, &txtRct);
		SDL_DestroyTexture(button->text.texture);
	}
}
	
void GUI::GUIUpdateButton( GUIButton* button, Input::MouseInput *mouse)
{
	if(mouse->pos.x < button->border.x + button->border.w &&
		mouse->pos.x > button->border.x &&
		mouse->pos.y < button->border.y + button->border.h &&
		mouse->pos.y > button->border.y )
	{
		button->hovering = true;

		if(mouse->leftButton.isDown)
		{
			if(button->OnClicked)
			{
				button->OnClicked();
			}
		}
	}
	else
	{
		button->hovering = false;
	}
}

void GUI::GUIUpdateWindow( GUIWindow* window, Input::MouseInput *mouse)
{
	if(window->isActive)
	{
		if(mouse->pos.x < window->border.x + window->border.w &&
			mouse->pos.x > window->border.x &&
			mouse->pos.y < window->border.y + window->border.h &&
			mouse->pos.y > window->border.y )
		{
			if(mouse->leftButton.isDown)
			{
				uint32 xoffset = mouse->pos.x - window->border.x;
				uint32 yoffset = mouse->pos.y - window->border.y;

				window->border.x = mouse->pos.x - (window->border.w / 2);
				window->border.y = mouse->pos.y - 10;
				window->titleBar.x = window->border.x;
				window->titleBar.y = window->border.y;
				window->exitButton.border.x = window->border.x + window->border.w - 10;
				window->exitButton.border.y = window->border.y;
			}
		}

		GUIUpdateButton(&window->exitButton, mouse);
	}
}

void GUI::DrawGUIText(SDL_Renderer* renderer, GUIText* gui)
{
	if (gui->texture)
	{
		SDL_Rect rect;
		rect.x = gui->pos.x;
		rect.y = gui->pos.y;
		
		SDL_QueryTexture(gui->texture, 0, 0, &rect.w, &rect.h);
		
		gui->dims.x = rect.w;
		gui->dims.y = rect.h;

		SDL_RenderCopy(renderer, gui->texture, 0, &rect);
		SDL_DestroyTexture(gui->texture);
	}
}

void OnWindowExit(void)
{

}

void GUI::CreateGUIWindow(GUIWindow* window, int x, int y, SDL_Texture* title, SDL_Texture* text )
{
	if(title && text)
	{
		window->title.texture = title;
		window->text.texture = text;

		int titleW = 0, textW = 0;
		int titleH = 0, textH = 0;

		SDL_QueryTexture(title, 0, 0, &titleW, &titleH);
		SDL_QueryTexture(text, 0, 0, &textW, &textH);

		window->border.x = x;
		window->border.y = y;
		window->border.w = textW + 20;
		window->border.h = ((textH + titleH) * 2) + 20;

		window->titleBar.x = window->border.x;
		window->titleBar.y = window->border.y;
		window->titleBar.w = window->border.w;
		window->titleBar.h = titleH + 5;

		window->exitButton.border.x = window->border.x + window->border.w - 20;
		window->exitButton.border.y = window->border.y;
		window->exitButton.border.w = 20;
		window->exitButton.border.h = window->titleBar.h;

		window->exitButton.OnClicked = OnWindowExit;

		window->title.pos.x = window->titleBar.x + 5;
		window->title.pos.y = window->titleBar.y + 5;
		window->title.dims.x = titleW;
		window->title.dims.y = titleH;

		window->text.pos.x = window->titleBar.x + 10;
		window->text.pos.y = window->titleBar.y + window->titleBar.h + 10;
		window->text.dims.x = textW;
		window->text.dims.y = textH;
	}
}

void GUI::CreateGUIButton(GUIButton* button, int x, int y, SDL_Texture* text, void (*OnClick)(void))
{	
	button->text.texture = text;
	SDL_QueryTexture(button->text.texture, 0, 0, &button->border.w, &button->border.h);

	button->border.x = x;
	button->border.y = y;
	button->border.w += 10;
	button->border.h += 10;

	button->OnClicked = OnClick;

	button->text.pos.x = x + 5;
	button->text.pos.y = y + 5;
	button->text.dims.x = button->border.w - 10;
	button->text.dims.y = button->border.h - 10;
}

void GUI::DrawGUIWindow(SDL_Renderer* renderer, GUIWindow* window)
{
	if(window->isActive)
	{
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderFillRect(renderer, &window->border);
	
		SDL_SetRenderDrawColor(renderer, 20, 50, 255, 255);
		SDL_RenderFillRect(renderer, &window->titleBar);
		
		if(window->exitButton.hovering)
		{
			SDL_SetRenderDrawColor(renderer, 50, 50, 255, 200);
		}
		else
		{
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 200);
		}

		SDL_RenderFillRect(renderer, &window->exitButton.border);

		DrawGUIText(renderer, &window->title);
		DrawGUIText(renderer, &window->text);
	}
}