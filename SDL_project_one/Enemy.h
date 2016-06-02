#ifndef ENEMY_H
#define ENEMY_H

#include "Entity.h"
#include "Path.h"
#include "Animation.h"

class Enemy : public Entity
{
	friend class Entity;
	friend class GameManager;
	friend class Player;
public:
	Enemy(void);
	Enemy(PathNode* newTarget);
	~Enemy(void);

	void Draw(SDL_Renderer* renderer) override;
	void Update(real32 dt) override;
	bool CollisionAABB(Entity* B) override;
	void HandleCollision(Entity* B) override;

protected:
	PathNode* target;
	real32	speed;
	vec2	dir;
	bool	isGrounded;
	bool	isAlive;

	int32				facingDir;
	uint32				prevTime;
	uint32				frame;
	AnimInfo			anim;
	bool				animate;
};

#endif