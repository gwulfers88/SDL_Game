#include "Player.h"

Player::Player(void)
{
	animate = false;
	isJumping = false;
	isGrounded = false;
	isCrouching = false;

	frame = 0;
	prevTime = 0;
	
	anim.frameRate = 6;
	anim.maxFrame = 4;
	anim.clipDims = Vec2(39, 39);
	
	facingDir = 0;
}

Player::~Player(void)
{

}

void Player::Update(void)
{
	Entity::Update();

	if(animate)
	{
		uint32 currTime = GetTickCount();

		if(currTime - prevTime > 1000 / anim.frameRate )
		{
			frame++;
			frame %= anim.maxFrame;
			prevTime = currTime;
		}
	}
	else 
		frame = 0;
}

void Player::Move(vec2 dir)
{
	pos += dir * speed;
}

void Player::Draw(SDL_Renderer* renderer)
{
	SDL_Rect dest;
	dest.x = (int32)pos.x;
	dest.y = (int32)pos.y;
	dest.w = (int32)dims.x;
	dest.h = (int32)dims.y;

	SDL_Rect src;
	
	src.w = anim.clipDims.x;
	src.h = anim.clipDims.y;
	src.x = facingDir * src.w;
	src.y = frame * src.h;
	
	SDL_Point c;
	c.x = center.x;
	c.y = center.y;

	SDL_RenderCopyEx(renderer, texture, &src, &dest, 0, &c, flip);
}