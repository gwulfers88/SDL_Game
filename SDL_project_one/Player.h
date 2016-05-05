#ifndef PLAYER_H
#define PLAYER_H

#include "Entity.h"
#include <map>

struct AnimInfo
{
	uint32 frameRate;
	uint32 maxFrame;
	vec2 clipDims;
};

class Player : public Entity
{
	friend class GameManager;
public:
	Player(void);
	~Player(void);

	void Update(void) override;
	void Move(vec2 dir);
	void Draw(SDL_Renderer* renderer) override;

protected:
	SDL_RendererFlip flip;

	uint32 frame;
	AnimInfo anim;

	uint32 prevTime;

	bool animate;
	bool isJumping;
	bool isGrounded;
	bool isCrouching;
	
	int32 facingDir;

};

#endif