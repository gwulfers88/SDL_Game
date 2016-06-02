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
#include "TileMap.h"
#include "vec2.h"

class Player;
class Enemy;

class Entity
{
	friend class GameManager;
	friend struct RenderObject;
	friend class Player;
	friend class Enemy;
public:
	Entity(void);
	Entity(SDL_Texture* texture, vec2 pos, vec2 dims);
	~Entity(void);

	void CalculateMidpoint();
	virtual void Draw(SDL_Renderer* renderer);
	virtual void Update(real32 dt);
	virtual bool CollisionAABB(Entity* B);
	virtual void HandleCollision(Entity* B);

protected:
	SDL_Texture*	texture;	//Each Entity will have its own texture.
	SDL_Rect		colRect;	//
	bool right;
	vec2			pos;		//Position vector
	vec2			dims;		//Dimension vector
	vec2			center;		//
	int32			tileID;		//
	int32			layer;		//
	int8			type[8];	//
};

#endif