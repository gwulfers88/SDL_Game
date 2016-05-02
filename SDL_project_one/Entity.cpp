/*
	Programmer: George Wulfers
	Class:		Coding 2
	Date:		4/12/2016
	--------------------------
	File:	Sprite.cpp

	Desc: Drawable sprite object
	that holds a pointer and dimensions
	to the texture.
*/

#include "Entity.h"

Entity::Entity(void)
{
	this->texture = 0;
	ZeroMemory(&pos, sizeof(vec2));		//Init struct to 0 for all of its members.
	ZeroMemory(&dims, sizeof(vec2));
	ZeroMemory(&center, sizeof(vec2));
	speed = 3.0f;
}

Entity::Entity(SDL_Texture* texture, vec2 pos, vec2 dims)
{
	this->texture = texture;
	this->pos = pos;
	this->dims = dims;
	CalculateMidpoint();
}

//Release any memory used.
Entity::~Entity(void)
{

}

void Entity::CalculateMidpoint(void)
{
	center.x = (dims.x * 0.5f);
	center.y = (dims.y * 0.5f);
}

void Entity::Update(void)
{
	if(pos.x > SCREEN_WIDTH)
	{
		pos.x = 0;
	}
	else if(pos.x + dims.x < 0 )
	{
		pos.x = SCREEN_WIDTH;
	}

	if(pos.y > SCREEN_HEIGHT)
	{
		pos.y = 0;
	}
	else if(pos.y < 0 )
	{
		pos.x = SCREEN_HEIGHT;
	}
}

//Used to draw each object.
void Entity::Draw(SDL_Renderer* renderer)
{
	SDL_Rect dest;
	dest.x = (int32)pos.x;
	dest.y = (int32)pos.y;
	dest.w = (int32)dims.x;
	dest.h = (int32)dims.y;

	SDL_RenderCopy(renderer, texture, 0, &dest);
}