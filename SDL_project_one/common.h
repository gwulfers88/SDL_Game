/*
	Programmer: George Wulfers
	Class:		Coding 2
	Date:		4/12/2016
	--------------------------
	File:	Common.h

	Desc: defines and other common
	functions or definitons that systems
	would use.
*/

#ifndef COMMON_H
#define COMMON_H

#include "wulfdefs.h"

//Appends to the end of the string
void COM_strcat(int8* dest, const int8* src);
//Copy string from src to dest
void COM_strcpy(int8* dest, const int8* src);
//Copy desired size of string from src to dest
void COM_strncpy(int8* dest, const int8* src, int32 count);
//Compare two strings. returns 0 = same, 1 = str2 > str1, -1 = str1 > str2
int32 COM_strcmp(const int8* str1, const int8* str2);
//Get size of string
int32 COM_strlen(const int8* str);
//Print string
void COM_print(int8* str);
//prints formatted strings
void COM_printf(int8* fmt, ...);

#endif