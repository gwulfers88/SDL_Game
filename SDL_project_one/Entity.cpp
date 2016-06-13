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
	tileID = -1;
	ZeroMemory(&pos, sizeof(vec2));		//Init struct to 0 for all of its members.
	ZeroMemory(&dims, sizeof(vec2));
	ZeroMemory(&center, sizeof(vec2));
	ZeroMemory(&colRect, sizeof(colRect));
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

void Entity::Update(real32 dt)
{
	if(!COM_strcmp(type, "floor"))
	{
		colRect.x = pos.x;
		colRect.y = pos.y;
		colRect.w = dims.x;
		colRect.h = center.y;
	}
	else if(!COM_strcmp(type, "spike"))
	{
		colRect.x = pos.x;
		colRect.y = pos.y + center.y;
		colRect.w = dims.x;
		colRect.h = center.y;
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

	SDL_Point c;
	c.x = (int32)center.x;
	c.y = (int32)center.y;

	if(tileID >= 0)
	{
		uint32 tileX = (tileID % 7);
		uint32 tileY = tileID / 7;

		SDL_Rect src;
		src.w = 128;
		src.h = 128;
		src.x = tileX * src.w;
		src.y = tileY * src.h;

		SDL_RenderCopyEx(renderer, texture, &src, &dest, 0, &c, SDL_FLIP_NONE);
	}
	else
	{
		SDL_RenderCopyEx(renderer, texture, 0, &dest, 0, &c, SDL_FLIP_NONE);
	}
}

bool Entity::CollisionAABB(Entity* B)
{
	if(	pos.x < B->colRect.x + B->colRect.w &&
		pos.x + dims.x > B->colRect.x &&
		pos.y < B->colRect.y + B->colRect.h &&
		pos.y + dims.y > B->colRect.y)
	{
		return true;
	}

	return false;
}

void Entity::HandleCollision(Entity* B)
{
	//Entity will do nothing!!
}