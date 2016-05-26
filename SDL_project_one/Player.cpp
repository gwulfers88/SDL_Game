#include "Player.h"

Player::Player(void)
{
	animate = false;
	isJumping = false;
	isGrounded = false;
	isCrouching = false;
	isAlive = true;

	speed = 100.0f;
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

void Player::Update(real32 dt)
{
	if(isAlive)
	{
		pos += dir * speed * dt;

		if(dir.x > 0 )
		{
			animate = true;
			facingDir = 1;
		}
		else if( dir.x < 0 )
		{
			animate = true;
			facingDir = 0;
		}
		else if(dir.x == 0)
		{
			animate = false;
		}
	
		Entity::Update(dt);

		if(animate)
		{
			uint32 currTime = GetTickCount();

			if(currTime - prevTime > anim.frameRate / dt )
			{
				frame++;
				frame %= anim.maxFrame;
				prevTime = currTime;
			}
		}
		else 
			frame = 0;
	}
	else
	{
		facingDir = 9;
		frame = 3;
	}
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
	c.x = (int32)center.x;
	c.y = (int32)center.y;

	SDL_RenderCopyEx(renderer, texture, &src, &dest, 0, &c, SDL_FLIP_NONE);
}

bool Player::CollisionAABB(Entity* B)
{
	if(!COM_strcmp(B->type, "enemy"))
	{
		if(pos.x + 10 < B->pos.x + B->dims.x &&
			pos.x + dims.x - 32 > B->pos.x &&
			pos.y < B->pos.y + B->dims.y &&
			pos.y + dims.y > B->pos.y)
		{
			return true;
		}
	}
	else 
	{
		if(pos.x + 10 < B->colRect.x + B->colRect.w &&
			pos.x + dims.x - 32 > B->colRect.x &&
			pos.y < B->colRect.y + B->colRect.h &&
			pos.y + dims.y > B->colRect.y)
		{
			return true;
		}
	}

	return false;
}

void Player::HandleCollision(Entity* B)
{
	if(!COM_strcmp(B->type, "floor" ))
	{
		if( pos.y > B->colRect.y + B->colRect.h &&
			pos.y + center.y > B->colRect.y + B->colRect.h)						//player Top
		{
			real32 colOffsetY = (B->colRect.y + B->colRect.h) - (pos.y);		//Calculate how far in we went to go back by that much.
			pos.y += colOffsetY - 2;											//Go back
		}
		else if(pos.y + center.y < B->colRect.y &&
				pos.y + dims.y < B->colRect.y + B->colRect.h)					//player Bottom
		{
			real32 colOffsetY = (pos.y + dims.y) - (B->colRect.y);				//Calculate how far in we went to go back by that much.
			pos.y -= colOffsetY - 2;											//Go back
			isGrounded = true;
		}
		else if(pos.x + dims.x - 32 < B->colRect.x + B->colRect.w &&
				pos.x + center.x < B->colRect.x)								//player Right
		{
			real32 colOffsetX = (pos.x + dims.x - 32) - (B->colRect.x);			//Calculate how far in we went to go back by that much.
			pos.x -= colOffsetX;												//Go back
		}
		else if(pos.x + 10 > B->colRect.x + B->colRect.w)						//player Left
		{
			real32 colOffsetX = (B->pos.x + B->dims.x) - (pos.x + 10);			//Calculate how far in we went to go back by that much.
			pos.x += colOffsetX;												//Go back
		}
	}
	else if(!COM_strcmp(B->type, "spike" ) || 
			!COM_strcmp(B->type, "enemy" ))
	{
		isAlive = false;
	}
}