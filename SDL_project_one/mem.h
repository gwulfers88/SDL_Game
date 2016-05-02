/*
	Programmer: George Wulfers
	Class:		Coding 2
	Date:		4/22/2016
	--------------------------
	File:	mem.h

	Desc: This is the memory allocation system.
	This system is incharge of allocating a big chunk of data
	for the game to use. It then splits this chunk into smaller pieces.

	structure of the memory

	Every allocation made with this system will align the bytes to a 16 byte boundary.
	This is to ensure that the system can read memroy as fast as possible.

	With in every allocation you will have a header object. This object contains information to make sure that this
	memory block is valid. It is also 16 bytes aligned.

	---------------TOP OF MEMORY-----------------
	
	To be discussed later. Currently all memory is
	stored from the bottom up. 

	resource allocations mixed between entities.
	entity allocations
	system allocations
	---------------BOTTOM OF MEMORY-----------------

	use MemCheck(void) to get a print out of the memory map.
*/

#ifndef MEM_H
#define MEM_H

#include "common.h"

#define MEMORY_SENTINAL 0x1df0013d	//Borrowed this from id Software.
const int32 MEMORY_SIZE = Kilobytes(16);

void MemoryInit(void *Buffer, int32 BufferSize);
void *MemAllocName(uint32 RequestSize, int8* RequestName);
void *MemAlloc(uint32 RequestSize);
void FreeMemBlock(void);
void FreeToLowMark(int32 Mark);
void MemCheck(void);

//Allocates memory to the High part.
void* MemHighAllocName(uint32 RequestSize, int8* RequestName);
void* MemHighAlloc(uint32 RequestSize);
void FreeToHighMark( int32 Mark );

#endif