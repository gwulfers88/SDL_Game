/*
	Programmer: George Wulfers
	Class:		Coding 2
	Date:		4/12/2016
	--------------------------
	File:	wulfdefs.h

	Desc: defines definitons that systems
	would use.
*/

#ifndef WULF_DEFS_H
#define WULF_DEFS_H

#include <SDL\SDL.h>
#include <SDL\SDL_image.h>
#include <stdio.h>
#include <iostream>
#include <Windows.h>
#include <fstream>
#include <assert.h>
#include <VLD\vld.h>

#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")
#pragma comment(lib, "SDL2_image.lib")
#pragma comment(lib, "SDL2_mixer.lib")
#pragma comment(lib, "SDL2_ttf.lib")
#pragma comment(lib, "SDL2test.lib")

#define _CRT_SECURE_NO_WARNINGS 1

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

#define Kilobytes( size ) ( size * 1024 );
#define Megabytes( size ) ( Kilobytes( size ) * 1024 );
#define Gigabytes( size ) ( Megabytes( size ) * 1024 );

using namespace std;

typedef char int8;
typedef short int16;
typedef int int32;
typedef long long int64;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;

typedef float real32;
typedef double real64;

#endif