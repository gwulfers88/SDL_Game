/*
	Programmer: George Wulfers
	Class:		Coding 2
	Date:		4/12/2016
	--------------------------
	File:	main.cpp

	Desc: starting point of the program.
*/

#include "mem.h"
#include "GameManager.h"

int main( int argc, char* args[] )
{
	MemoryInit(malloc(MEMORY_SIZE), MEMORY_SIZE);	//Initialize memory chunk allocations
	
	GameManager game;

	game.Run();

	return 0;
}