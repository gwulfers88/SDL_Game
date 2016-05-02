/*
	Programmer: George Wulfers
	Class:		Coding 2
	Date:		4/12/2016
	--------------------------
	File:	Window.h

	Desc: Creates an SDL window and 
	holds the handle to that window.
*/

#ifndef WINDOW_H
#define WINDOW_H

#include "common.h"

class Window
{
public:
	Window(void);
	~Window(void);

	bool Init_Window(int8* title, int32 width, int32 height, uint32 flags);

	SDL_Window* GetWindow() const;
	
	int32 GetWidth() const;
	int32 GetHeight() const;

	void Cleanup();
private:
	SDL_Window* window;
	
	int32 width;
	int32 height;
	int32 wndHz;
};

#endif