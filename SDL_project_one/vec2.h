/*
	Programmer: George Wulfers
	Class:		Coding 2
	Date:		4/13/2016
	--------------------------
	File:	vec2.h

	Desc: 2 dimensional vector.
*/

#ifndef VEC2_H
#define VEC2_H

#include "common.h"

struct vec2
{
	real32 x;
	real32 y;
};

inline vec2 Vec2( real32 x, real32 y)
{
	vec2 result;
	result.x = x;
	result.y = y;

	return result;
}

inline vec2 operator+(vec2 a, vec2 b)
{
	vec2 res;
	res.x = a.x + b.x;
	res.y = a.y + b.y;

	return res;
}

inline vec2& operator+=(vec2& a, vec2 b)
{
	a = b + a;

	return a;
}

inline vec2 operator*(vec2 a, real32 b)
{
	vec2 res;
	res.x = a.x * b;
	res.y = a.y * b;

	return res;
}

inline vec2 operator*(vec2 a, vec2 b)
{
	vec2 res;
	res.x = a.x * b.x;
	res.y = a.y * b.y;

	return res;
}

inline vec2& operator*=(vec2& a, vec2 b)
{
	a = b * a;

	return a;
}

inline vec2& operator*=(vec2& a, float b)
{
	a = a * b;

	return a;
}

inline real32 Distance( vec2 a, vec2 b)
{
	real32 result;
	result = sqrt(((a.x - b.x)*(a.x - b.x)) + ((a.y - b.y) * (a.y - b.y)));
	return result;
}

#endif