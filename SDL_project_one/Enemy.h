#ifndef ENEMY_H
#define ENEMY_H

#include "Entity.h"

class Enemy : public Entity
{
	friend class Entity;
	friend class GameManager;
public:
	Enemy(void);
	Enemy(Entity* newTarget);
	~Enemy(void);

	void Draw(SDL_Renderer* renderer) override;
	void Update(real32 dt) override;
	bool CollisionAABB(Entity* B) override;
	void HandleCollision(Entity* B) override;

protected:
	Entity* target;
	real32	speed;
	vec2	dir;
	bool	isGrounded;
	bool	isAlive;
};

#endif