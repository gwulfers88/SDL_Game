#ifndef PATHNODE_H
#define PATHNODE_H

#include "common.h"
#include "vec2.h"

struct PathNode
{
	vec2 pos;
	struct PathNode* next;
	struct PathNode* prev;
};

#endif