#ifndef PLAYER_H
#define PLAYER_H

#include "Entity.h"

struct AnimInfo
{
	uint32 frameRate;
	uint32 maxFrame;
	vec2 clipDims;
};	//struct size: 16 bytes

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

protected:
	vec2				dir;
	int32				facingDir;
	uint32				prevTime;
	uint32				frame;
	real32				speed;
	AnimInfo			anim;
	bool				animate;
	bool				isJumping;
	bool				isGrounded;
	bool				isCrouching;
	bool				isAlive;
};

#endif