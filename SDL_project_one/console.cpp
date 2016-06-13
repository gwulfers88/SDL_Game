#include "console.h"
#include "mem.h"

#define MAX_ARGS 4

uint32 argsCount = 0;
int8* args[MAX_ARGS];

void InitCommandline()
{
	for(int i = 0; i < MAX_ARGS; i++)
	{
		args[i] = (int8*)MemHighAllocName(MAX_PATH, "cmdline");
	}
}

void clearArgs()
{
	for(int i = 0; i < MAX_ARGS; i++)
	{
		COM_strncpy(args[i], "", MAX_PATH);
	}
}

void commandParser(int8* buffer)
{
	clearArgs();

	argsCount = 0;
	int count = 0;

	if(*buffer == '/')
	{
		while(*buffer)
		{
			if(argsCount >= 4)
				break;

			*args[argsCount]++ = *buffer++;
			count++;

			if(*buffer == ' ')
			{
				*args[argsCount] = 0;
				args[argsCount] -= count;
				count = 0;
				buffer++;
				argsCount++;
			}
		}

		*args[argsCount] = 0;
		args[argsCount] -= count;
	}
}

bool checkCommand(const int8* command)
{
	return (!COM_strcmp(command, args[0]));
}

bool checkParam(const int8* parameter, uint32 index)
{
	if(index >= MAX_ARGS)
		index = MAX_ARGS - 1;

	return (!COM_strcmp(parameter, args[index]));
}

int8* getParam(uint32 index)
{
	if(index >= MAX_ARGS)
		index = MAX_ARGS - 1;

	return args[index];
}

void showDebug(Console *console, uint32 flags)			//sets different parameters
{
	console->showDebug ^= flags;
}

void setLayers(Console *console, uint32 flags)			//sets different parameters
{
	console->layerFlags ^= flags;
}