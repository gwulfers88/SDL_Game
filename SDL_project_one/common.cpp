#include "common.h"

//Appends to the end of the string
void COM_strcat(int8* dest, const int8* src)
{
	while(*dest)
	{
		dest++;
	}

	while(*src)
	{
		*dest++ = *src++;
	}
	*dest++ = 0;
}

//Appends tot he end of the string and inserts 'char' after
void COM_strcat(int8* dest, const int8* src, char c)
{
	while(*dest)
	{
		dest++;
	}

	while(*src)
	{
		*dest++ = *src++;
	}

	*dest++ = c;
}

//Copy string from src to dest
void COM_strcpy(int8* dest, const int8* src)
{
	while(*src)
	{
		*dest++ = *src++;
	}

	*dest++ = 0;	//Null terminate string
}

//Copy desired size of string from src to dest
void COM_strncpy(int8* dest, const int8* src, int32 count)
{
	while(*src && count--)
	{
		*dest++ = *src++;
	}

	if(count)
		*dest++ = 0;
}

//Compare two strings. returns 0 = same, -1 = not same
int32 COM_strcmp(const int8* str1, const int8* str2)
{
	while(1)
	{
		if(*str1 != *str2)
			return -1;		//not same
		if(!*str1)
			return 0;		//same
		*str1++;
		*str2++;
	}

	return -1;
}

//Get size of string
int32 COM_strlen(const int8* str)
{
	int32 count = 0;

	while(str[count])
		count++;

	return count;
}

//Print string
void COM_print(int8* str)
{
	while(*str)
	{
		cout << *str++;
	}
}

//prints formatted strings
void COM_printf(int8* fmt, ...)
{
	va_list argptr;
	int8 text[1024];

	va_start(argptr, fmt);
	vsprintf(text, fmt, argptr);
	va_end(argptr);

	COM_print(text);
}

//Converts string from A to a
void COM_convertLower(int8* str)
{
	while(*str)
	{
		if(*str >= 'A' && *str <= 'Z')
			*str += 32;
		
		str++;
	}
}