#include "Player.h"
#include "Enemy.h"

Player::Player(void)
{
	isGrounded = false;
	isJumping = false;
	isAlive = true;

	lives = 3;
	speed = 120.0f;
	frame = 0;
	prevTime = 0;
	
	state = "spawn";
	spawned = false;
	facing = SDL_FLIP_NONE;

	/*anim.frameRate = 6;
	anim.maxFrame = 4;
	anim.clipDims = Vec2(39, 39);
	*/

	jumpTime = 0;
}

Player::~Player(void)
{
	anim.clear();
}

void Player::Move(float dt)
{
	if(isAlive)
	{
		pos += dir * speed * dt;
	}
}

void Player::Update(real32 dt)
{
	if(isAlive)
	{
		if(spawned)
		{
			if(lives == 0)
			{
				isAlive = false;
			}

			if(dir.x > 0 )
			{
				if(COM_strcmp(state.c_str(), "jump"))
				{
					state = "walk";
				}
				facing = SDL_FLIP_NONE;
			}
			else if( dir.x < 0 )
			{
				if(COM_strcmp(state.c_str(), "jump"))
				{
					state = "walk";
				}
				facing = SDL_FLIP_HORIZONTAL;
			}
			else if(dir.x == 0)
			{
				if(COM_strcmp(state.c_str(), "jump"))
				{
					state = "idle";
				}
			}

			if(frame >= (anim[state]->maxFrame))
			{
				frame = 0;
			}

			uint32 currTime = GetTickCount();

			if(currTime - prevTime > 1000 / anim[state]->frameRate )
			{
				if(state == "jump")
				{
					if(frame < anim[state]->maxFrame - 1)
					{
						frame++;
					}
				}
				else
				{
					frame++;
					frame %= (anim[state]->maxFrame);
				}

				prevTime = currTime;
			}
		}
		else
		{
			uint32 currTime = GetTickCount();

			if(currTime - prevTime > 1000 / anim[state]->frameRate )
			{
				if(frame < anim[state]->maxFrame - 1)
				{
					frame++;
					prevTime = currTime;
				}
				else
				{
					spawned = true;
				}
			}
		}
	}
	else
	{
		state = "death";

		uint32 currTime = GetTickCount();

		if(currTime - prevTime > 1000 / anim[state]->frameRate )
		{
			if(frame < anim[state]->maxFrame - 1)
			{
				frame++;
				prevTime = currTime;
			}
		}
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
	
	src.w = anim[state]->clipDims.x;
	src.h = anim[state]->clipDims.y;
	src.x = frame * src.w;
	src.y = anim[state]->clipPos.y;
	
	SDL_Point c;
	c.x = (int32)center.x;
	c.y = (int32)center.y;

	SDL_RenderCopyEx(renderer, texture, &src, &dest, 0, &c, facing);
}

bool Player::CollisionAABB(Entity* B)
{
	if(!COM_strcmp(B->type, "enemy"))
	{
		if( pos.x + 10 < B->pos.x + B->dims.x - 32 &&
			pos.x + dims.x - 32 > B->pos.x + 10 &&
			pos.y < B->pos.y + B->dims.y &&
			pos.y + dims.y > B->pos.y + 32)
		{
			return true;
		}
	}
	else 
	{
		if( pos.x + 10 < B->colRect.x + B->colRect.w &&
			pos.x + dims.x - 40 > B->colRect.x &&
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
	if(isAlive)
	{
		if(!COM_strcmp(B->type, "floor" ))
		{
			if( pos.y > B->colRect.y + B->colRect.h &&
				pos.y + center.y > B->colRect.y + B->colRect.h)						//player Top
			{
				real32 colOffsetY = (B->colRect.y + B->colRect.h) - (pos.y);		//Calculate how far in we went to go back by that much.
				pos.y += colOffsetY - 2;											//Go back
			}
			else if(pos.y < B->colRect.y &&
					pos.y + dims.y - 5< B->colRect.y + B->colRect.h)					//player Bottom
			{
				real32 colOffsetY = (pos.y + dims.y- 5) - (B->colRect.y);				//Calculate how far in we went to go back by that much.
				pos.y -= colOffsetY - 2;											//Go back
				
				if(spawned)
				{
					state = "idle";
				}

				isGrounded = true;
			}
			else if(pos.x + dims.x - 40< B->colRect.x + B->colRect.w &&
					pos.x + center.x < B->colRect.x)								//player Right
			{
				real32 colOffsetX = (pos.x + dims.x - 40) - (B->colRect.x);			//Calculate how far in we went to go back by that much.
				pos.x -= colOffsetX;												//Go back
			}
			else if(pos.x + 10 > B->colRect.x + B->colRect.w)						//player Left
			{
				real32 colOffsetX = (B->pos.x + B->dims.x) - (pos.x + 10);			//Calculate how far in we went to go back by that much.
				pos.x += colOffsetX;												//Go back
			}
		}
		else if(!COM_strcmp(B->type, "spike" ))
		{
			isAlive = false;
			frame = 0;
		}
		else if(!COM_strcmp(B->type, "enemy" ))
		{
			Enemy* e = (Enemy*)B;
		
			if(e && e->isAlive)
			{
				if( pos.y + center.y < B->pos.y + 10 &&
					pos.y + dims.y < B->pos.y + B->dims.y)							//player Bottom
				{
					e->isAlive = false;
					jumpTime = 1.55f;

					return;
				}
				else if(pos.x + dims.x - 32 < B->pos.x + B->dims.x - 32&&
						pos.x + center.x < B->pos.x + 10)								//player Right
				{
					lives--;
					jumpTime = 1.0f;
					pos.x -= 20;
					return;
				}
				else if(pos.x + 10 > B->pos.x + B->dims.x - 32)						//player Left
				{
					lives--;
					jumpTime = 1.0f;
					pos.x += 20;
					return;
				}
			}
		}
	}
}