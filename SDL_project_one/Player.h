#ifndef PLAYER_H
#define PLAYER_H

#include "Entity.h"
#include "Animation.h"

class Player : public Entity
{
	friend class GameManager;
	friend class Enemy;
public:
	Player(void);
	~Player(void);
	
	void Update(real32 dt) override;
	void Draw(SDL_Renderer* renderer) override;
	bool CollisionAABB(Entity* B) override;
	void HandleCollision(Entity* B) override;
	void Move(float dt);

protected:
	vec2				dir;
	int32				facingDir;
	uint32				prevTime;
	uint32				frame;
	real32				speed;
	real32				jumpTime;
	AnimInfo			anim;
	bool				animate;
	bool				isGrounded;
	int32				lives;
	bool				isAlive;
};

#endif