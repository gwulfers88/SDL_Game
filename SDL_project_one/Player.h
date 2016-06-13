#ifndef PLAYER_H
#define PLAYER_H

#include "Entity.h"
#include "Animation.h"
#include <string>
#include <map>

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
	bool spawned;
	SDL_RendererFlip facing;
	vec2				dir;
	uint32				prevTime;
	uint32				frame;
	real32				speed;
	real32				jumpTime;
	string				state;
	map<string, AnimInfo*>	anim;
	bool				isGrounded;
	bool				isJumping;
	int32				lives;
	bool				isAlive;
};

#endif