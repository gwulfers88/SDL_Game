/*
	Programmer: George Wulfers
	Class:		Coding 2
	Date:		4/12/2016
	--------------------------
	File:	Sprite.h

	Desc: Drawable sprite object 
	that holds a pointer and dimensions 
	to the texture.
*/

#ifndef ENTITY_H
#define ENTITY_H

#include "common.h"
#include "vec2.h"

class Entity
{
	friend class GameManager;
	friend struct RenderObject;
public:
	Entity(void);
	Entity(SDL_Texture* texture, vec2 pos, vec2 dims);
	~Entity(void);

	void CalculateMidpoint();
	virtual void Draw(SDL_Renderer* renderer);
	virtual void Update(void);

protected:
	SDL_Texture* texture;	//Each sprite Object will have its own texture.
	vec2 pos;				//Position vector
	vec2 dims;				//Dimension vector
	vec2 center;
	int32 layer;
	int8 type[8];
};

#endif