#ifndef WOLF_CONSOLE_H
#define WOLF_CONSOLE_H

#include "common.h"
#include "mem.h"

#define MAX_ARGS 4

uint32 argsCount = 0;
int8* args[MAX_ARGS];

void InitCommandline()
{
	for(int i = 0; i < MAX_ARGS; i++)
	{
		args[i] = (int8*)MemHighAllocName(16, "cmdline");
	}
}

void clearArgs()
{
	for(int i = 0; i < MAX_ARGS; i++)
	{
		COM_strncpy(args[i], "", 16);
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

#endif