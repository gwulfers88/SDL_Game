#include "mem.h"

// Memory header information.
typedef struct
{
	int32 MemorySentinel;
	int32 MemorySize;
	int8 MemoryName[8];
} MemHeader;	//size of struct 16 bytes

//Memory Block for game
struct Memory
{
	void*  MemBase;			//Data
	uint32 MemOffset;		//Current Memory used. Base + MemOffset = Current Free space.
	uint32 MemHighOffset;	//Current Memory used. Base + MemSize - MemHighOffset.
	uint32 MemSize;			//Total Size on the heap
};	// size of struct 12 bytes

Memory Mem;

// Initializes memory
void MemoryInit(void *Buffer, int32 BufferSize)
{
	ZeroMemory(&Mem, sizeof(Memory));			//Clear out the struct to zero
	Mem.MemBase = Buffer;
	Mem.MemSize = BufferSize;
}

void * MemAlloc(uint32 RequestSize)
{
	return MemAllocName(RequestSize, "unknown");
}

/* 
	Allocates specific size in bytes into our memory space
	Memory size must be 16 byte aligned for faster lookup.
	Pass in a name for memory block.
*/
void * MemAllocName( uint32 RequestSize, int8* RequestName )
{
	//TODO(George): Make this system more robust.
	uint32 TotalSize = sizeof(MemHeader) + ((RequestSize + 15)& ~15);	//16 byte boundary only!!
	
	COM_printf("\nAllocating memory:\n\tRequested size: %d bytes\n", RequestSize);
	COM_printf("\tActual memory size allocated: %d bytes (16 byte aligned).\n", TotalSize);

	//Make sure we dont go over the Total Size allocated.
	assert(((Mem.MemOffset + TotalSize) < Mem.MemSize));

	//Size of memory is in bytes so make sure to cast down to 1 byte when doing pointer arithmetic to get to the next location in memory that us available.
	MemHeader* header = (MemHeader*)((int8*)Mem.MemBase + Mem.MemOffset);
	memset(header, 0, TotalSize);	//Zero out the memory.

	Mem.MemOffset += TotalSize;	//Add used memory size.
	
	//Fillout memory header file
	header->MemorySentinel = MEMORY_SENTINAL;
	header->MemorySize = TotalSize;
	COM_strcpy(header->MemoryName, RequestName);

	COM_printf("\tTotal size used: %d bytes out of %d bytes\n", (Mem.MemOffset + Mem.MemHighOffset), Mem.MemSize);
	COM_printf("\t%.02f percent memory used.\n", (((real32)(Mem.MemOffset + Mem.MemHighOffset) / (real32)Mem.MemSize) * 100));

	//return new allocation plus 16 bytes. (after memory header).
	return (void*)(header + 1);
}

void FreeToLowMark(int32 Mark)
{
	memset(((int8*)Mem.MemBase + Mark), 0, Mem.MemOffset - Mark);
	Mem.MemOffset = Mark;
}

//Allocates memory to the high part of our memory
void* MemHighAlloc(uint32 RequestSize)
{
	return MemHighAllocName(RequestSize, "unknown");
}

//Allocates memory to the High part.
void* MemHighAllocName(uint32 RequestSize, int8* RequestName)
{
	uint32 TotalSize = sizeof(MemHeader) + ((RequestSize + 15) & ~15);

	assert(((Mem.MemHighOffset + TotalSize) < (Mem.MemSize - Mem.MemHighOffset)));

	Mem.MemHighOffset += TotalSize;
	MemHeader* header = (MemHeader*)((int8*)Mem.MemBase + Mem.MemSize - Mem.MemHighOffset);

	memset(header, 0, TotalSize);

	header->MemorySentinel = MEMORY_SENTINAL;
	header->MemorySize = TotalSize;
	COM_strcpy(header->MemoryName, RequestName);

	return (void*)(header + 1);
}

void FreeToHighMark( int32 Mark )
{
	memset(((int8*)Mem.MemBase + Mem.MemSize - Mem.MemHighOffset), 0, Mem.MemHighOffset - Mark);
	Mem.MemHighOffset = Mark;
}

//Frees Full Memory Block
void FreeMemBlock(void)
{
	if(Mem.MemBase)
	{
		free(Mem.MemBase);	//Free memory block
		Mem.MemBase = 0;	//Set to zero
	}
}

//Check integrity of our memory allocations
void MemCheck(void)
{
	for( MemHeader* header = (MemHeader*)Mem.MemBase;
		(int8*)header != (int8*)Mem.MemBase + Mem.MemOffset; )
	{
		if(header->MemorySentinel != MEMORY_SENTINAL)
			assert(0);

		COM_printf("\n\tMemCheck(): SENTINAL [OK]: name: %s\n", header->MemoryName);

		if(header->MemorySize < 16 || (int8*)header - Mem.MemBase + header->MemorySize > Mem.MemSize)
			assert(0);

		COM_printf("\tMemCheck(): BLOCK SIZE [OK]: size: %d\n", header->MemorySize);

		header = (MemHeader*)((int8*)header + header->MemorySize);
	}
}