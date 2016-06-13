#include "LevelFile.h"

GLF* glf;

void glfInit()
{
	glf = (GLF*)MemAllocName(sizeof(GLF), "glf");
	COM_strncpy(glf->filename, "unknown", MAX_PATH);
	glf->next = 0;
}

void glfFindFiles()
{
	WIN32_FIND_DATAA ffd;

	HANDLE file = FindFirstFile("*.glf", &ffd);

	if(file)
	{
		COM_strncpy(glf->filename, ffd.cFileName, MAX_PATH);
		glf->fileSize.HighPart = ffd.nFileSizeHigh;
		glf->fileSize.LowPart = ffd.nFileSizeLow;
		
		while(FindNextFile(file, &ffd))
		{
			if(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
			}
			else
			{
				GLF* tmp = (GLF*)MemAllocName(sizeof(GLF), "glf");
				tmp->next = 0;

				COM_strncpy(tmp->filename, ffd.cFileName, MAX_PATH);
				tmp->fileSize.HighPart = ffd.nFileSizeHigh;
				tmp->fileSize.LowPart = ffd.nFileSizeLow;

				tmp->next = glf;
				glf = tmp;
			}
		}
	}
}

GLF* glfGetFile(char filename[MAX_PATH])
{
	GLF* walker = glf;

	while(walker != 0)
	{
		if(!COM_strcmp(walker->filename, filename))
		{
			return walker;
		}

		walker = walker->next;
	}

	return 0;
}

void glfPrintAll(void)
{
	for(GLF* walker = glf;
		walker;
		walker = walker->next)
	{
		COM_printf("filename: %s size: %i\n", walker->filename, walker->fileSize.LowPart);
	}
}
