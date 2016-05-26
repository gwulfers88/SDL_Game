#include "Enemy.h"
#include "Player.h"

Enemy::Enemy(void) : Entity()
{
	target = 0;
	speed = 50.0f;
	dir = Vec2(0, 0);
	isGrounded = false;
	isAlive = true;
}

Enemy::Enemy(Entity* newTarget) : Entity()
{
	target = newTarget;
	speed = 50.0f;
	dir = Vec2(0, 0);
	isGrounded = false;
	isAlive = true;
}

Enemy::~Enemy()
{
}

void Enemy::Draw(SDL_Renderer* renderer)
{
	Entity::Draw(renderer);	
}

void Enemy::Update(real32 dt)
{
	if(isAlive)
	{
		vec2 targetDir = {0};

		if(target)
		{
			Player* p = (Player*)target;
			
			if(p && p->isAlive)
			{
				if(Distance(target->pos, pos) < (target->dims.y) * 2)
				{
					targetDir = target->pos - this->pos;
					Normalize(targetDir);
					dir += targetDir;
				}
			}
			else
			{
				dir.x = 1;
			}
		}

		pos += speed * dir * dt;
	}
}

bool Enemy::CollisionAABB(Entity* B)
{
	if(!COM_strcmp(B->type, "player"))
	{
		if(pos.x < B->pos.x + B->dims.x &&
			pos.x + dims.x > B->pos.x &&
			pos.y < B->pos.y + B->dims.y &&
			pos.y + dims.y > B->pos.y)
		{
			return true;
		}
	}
	else 
	{
		if(pos.x < B->colRect.x + B->colRect.w &&
			pos.x + dims.x > B->colRect.x &&
			pos.y < B->colRect.y + B->colRect.h &&
			pos.y + dims.y > B->colRect.y)
		{
			return true;
		}
	}

	return false;
}

void Enemy::HandleCollision(Entity* B)
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
	else if(!COM_strcmp(B->type, "spike" ))
	{
		if(pos.y + dims.y > B->colRect.y)
			isAlive = false;
	}
}