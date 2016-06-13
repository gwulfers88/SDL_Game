#ifndef GLF_H
#define GLF_H

#include "common.h"
#include "mem.h"

//Level File storage
struct GLF
{
	char filename[MAX_PATH];
	LARGE_INTEGER fileSize;
	struct GLF* next;
};

void glfInit();	//Initialize the level file storage
void glfFindFiles();	//Locates all level files in directory and stores them.
GLF* glfGetFile(char filename[MAX_PATH]);	//returns a pointer to a file specified.
void glfPrintAll(void);		//prints all files in directory that are level files.

#endif