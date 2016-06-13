#ifndef WULF_CONSOLE_H
#define WULF_CONSOLE_H

#include "common.h"

//DEFINES
#define SHOW_DEBUG_NONE		0x00000000
#define SHOW_DEBUG_GRID		0x00000001
#define SHOW_DEBUG_COLISION 0x00000010
#define SHOW_DEBUG_LAYERS	0x00000100
#define SHOW_DEBUG_CMD		0x00001000
#define SHOW_DEBUG_PATH		0x00010000

#define DEBUG_LAYERS_PLAYER	0x00000000
#define DEBUG_LAYERS_ONE	0x00000001
#define DEBUG_LAYERS_TWO	0x00000010
#define DEBUG_LAYERS_THREE	0x00000100
#define DEBUG_LAYERS_FOUR	0x00001000

struct Console
{
	int8	input[256];
	uint32	showDebug;
	uint32	layerFlags;
	bool	isConsoleActive;
};

//FUNCTIONS PROTOTYPES
void InitCommandline();									//Initialize comand line
void clearArgs();										//clear argument list
void commandParser(int8* buffer);						//parse command input from user
bool checkCommand(const int8* command);					//check command
bool checkParam(const int8* parameter, uint32 index);	//check parameter
int8* getParam(uint32 index);							//Get the parameter
void showDebug(Console *console, uint32 flags);			//sets different parameters
void setLayers(Console *console, uint32 flags);			//sets different parameters
#endif