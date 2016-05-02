/*
	Programmer: George Wulfers
	Class:		Coding 2
	Date:		4/12/2016
	--------------------------
	File:	Window.cpp

	Desc: Creates an SDL window and 
	holds the handle to that window.
*/

#include "Window.h"

//Initialize all data to zero
Window::Window(void)
{
	width = 0;
	height = 0;
	wndHz = 0;
	window = 0;
}

//Clean up stuff
Window::~Window(void)
{
	Cleanup();
}

//Clean up stuff
void Window::Cleanup()
{
	if(window)
	{
		SDL_DestroyWindow(window);
	}
}

//Initialize window
bool Window::Init_Window(int8* title, int32 width, int32 height, uint32 flags)
{
	if(width && height)
	{
		this->width = width;
		this->height = height;
	}
	else
	{
		this->width = SCREEN_WIDTH;
		this->height = SCREEN_HEIGHT;
	}
	
	if(!flags)
		flags |= SDL_WINDOW_SHOWN;

	window = SDL_CreateWindow(title,
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		this->width, this->height, flags);

	if(!window)
		return false;

	return true;
}

//returns window ptr
SDL_Window* Window::GetWindow() const
{
	return window;
}

//returns window width
int32 Window::GetWidth() const
{
	return width;
}

//returns window height
int32 Window::GetHeight() const
{
	return height;
}
