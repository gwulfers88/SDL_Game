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
public:
	Player(void);
	~Player(void);
		
	void Move(vec2 dir);
	
	void Update(void) override;
	void Draw(SDL_Renderer* renderer) override;

protected:
	SDL_RendererFlip	flip;
	int32				facingDir;
	uint32				prevTime;
	uint32				frame;
	real32				speed;
	AnimInfo			anim;
	bool				animate;
	bool				isJumping;
	bool				isGrounded;
	bool				isCrouching;
};

#endif